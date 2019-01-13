# Java Code Example
This covers basic AWS DynamoDB interations using Java. **The goal of this guide is to provide enough information for you
to access the data without having to spend time on a deep-dive of the official documentation.**

## Requirements
1. Your favourite Java IDE (IntelliJ IDEA)
2. Java AWS SDK v.1.11.478
3. An AWS Access key, Secret Key, and Region

## AWS SDK Setup
You will need to add the following to your pom.xml file for your project
```xml
<dependencyManagement>
    <dependencies>
        <dependency>
            <groupId>com.amazonaws</groupId>
            <artifactId>aws-java-sdk-bom</artifactId>
            <version>1.11.478</version>
            <type>pom</type>
            <scope>import</scope>
        </dependency>
    </dependencies>
</dependencyManagement>

<dependencies>
    <dependency>
        <groupId>com.amazonaws</groupId>
        <artifactId>aws-java-sdk-ec2</artifactId>
    </dependency>
    <dependency>
        <groupId>com.amazonaws</groupId>
        <artifactId>aws-java-sdk-s3</artifactId>
    </dependency>
    <dependency>
        <groupId>com.amazonaws</groupId>
        <artifactId>aws-java-sdk-dynamodb</artifactId>
    </dependency>
</dependencies>
```
This will automatically import the SDK for you instead of having to deal with downloading .jars and managing them. You can
also use gradle if you prefer, but Maven is the fastest route since it is what Amazon directly supports and uses. You have the option of using the new
AWS 2.0 version, however, in the writing of this demonstration, 99% of the code samples available both on Amazon's documentation site
as well as third party tutorial sites used 1.11 for their examples. 2.0 is a **MASSIVE** change to the API's structure, so consider
this your warning. If you are already familiar with 2.0, then you can ignore most of this document.

## Working with DynamoDB and creating your client 
All examples are contained in a single file called SampleQueries.java. The API is powerful and gives you a massive amount of
options to query with, but for our purposes this should get you started.

In order to connect to the Database, you will need a client initialization method that can be called from your main method.
```java
private static void initDb() {
    BasicAWSCredentials awsCredentials = new BasicAWSCredentials(ACCESS_KEY, SECRET_KEY);

    client = AmazonDynamoDBClientBuilder.standard()
            .withCredentials(new AWSStaticCredentialsProvider(awsCredentials))
            .withRegion(Regions.US_EAST_1)
            .build();
}


public static void main(String[] args) throws Exception {
    initDb();

    ...
    ...
    ...
}
```

You will also want to catch two potential exceptions from the main method:

```java
public static void main(String[] args) throws Exception {
    initDb();

    try {
        // Query method calls go here, plus whatever else you want in your main method
        ...
        ...
        ...

    } catch (AmazonServiceException ase) {
                  System.out.println("Caught an AmazonServiceException, which means your request made it "
                          + "to AWS, but was rejected with an error response for some reason.");
                  System.out.println("Error Message:    " + ase.getMessage());
                  System.out.println("HTTP Status Code: " + ase.getStatusCode());
                  System.out.println("AWS Error Code:   " + ase.getErrorCode());
                  System.out.println("Error Type:       " + ase.getErrorType());
                  System.out.println("Request ID:       " + ase.getRequestId());
    } catch (AmazonClientException ace) {
        System.out.println("Caught an AmazonClientException, which means the client encountered "
                + "a serious internal problem while trying to communicate with AWS, "
                + "such as not being able to access the network.");
        System.out.println("Error Message: " + ace.getMessage());
    }
}
```
This will help you debug your application if you manage to get unhandled exceptions.


## Creating a QueryRequest
A `QueryRequest` is the object that represents your query. Each one can be customized depending on the information that
you're trying to fetch from the database. In the examples, each `QueryRequest` is defined and executed from the main
method, however, you have the ability to create methods in order to reuse your `QueryRequest` as needed.

Each `QueryRequest` must have a defined table name, otherwise it will return the entire database. Any values that are
used must be given a name and a type:
```java
QueryRequest queryRequest = new QueryRequest()
             .withTableName(TABLE_NAME)
             .withExpressionAttributeValues(
                     new HashMap<String, AttributeValue>() {{
                         put(":dId", new AttributeValue().withN(driveScenarioId));
                     }})
             .withKeyConditionExpression("driveid = :dId");
```
**dId** can be any name you want, however for `driveID` it is easiest to just use the above code.

The `.withN` is used for all numerical fields. This covers almost all of the data for this event. For the few string fields,
use `.withS` instead.

The basic (and required) filter is on the partition key. This must be included in every query:
```java
QueryRequest queryDriveScenarioRequest = new QueryRequest()
                    .withTableName(tableName)
                    .withKeyConditionExpression("driveid = :dId")
```

> DynamoDB works slightly differently than MySQL, MSSQL, or other popular databases that you may be used to. Because it
is a distributed cloud service on AWS, there is no direct IP address or single storage place. Each table has a composite
primary key that is defined by two sub keys. The first is the **partition key** which tells DynamoDB that rows are heavily
related and to try to store them together. In this data, `driveid` is the partition key (and represents the drive scenario).
The next part of the primary key is the **sort key** which is the default sorting used on the rows. **Every query must
use an equals `=` expression with the `partition key` (`driveid = :dId`). This means that any single query can only pull
records from one drive scenario.

A `FilterExpression` is an **optional** filter that can be applied to filter any column:
```java
QueryRequest queryAllGpsDataRequest = new QueryRequest()
        .withKeyConditionExpression("driveid = :dId")
        .withExpressionAttributeNames(attrNameAlias)
        .withFilterExpression("ColumnValue > 10");
```

Finally, to select which columns you want to retrieve for each row, use a `ProjectionExpression`:
```java
QueryRequest queryAllGpsDataRequest = new QueryRequest()
        .withKeyConditionExpression("driveid = :dId")
        .withProjectionExpression("GPS_Lat, GPS_Lon, GPS_Alt")
        .withExpressionAttributeNames(attrNameAlias);
```
If no `ProjectionExpression` is given, all columns will be returned.

## Running a QueryRequest
To run a `QueryRequest`, we need to use the `QueryResult` object:
```java
List<Map<String, AttributeValue>> results = new ArrayList<Map<String, AttributeValue>>();
QueryResult queryResult = queryResult = client.query(queryRequest);
results.addAll(queryResult.getItems());
```
You can format the result in a way that you want, you don't need to print it if you want to use that result for a calculation.

Queries to DynamoDB are limited to a certain size, measured in bytes. If you pull all the columns from the table, the
size limit is hit at about 400 rows. To work around this, if a query hits the limit before all the results are returned,
a `LastEvaluatedKey` attribute will have a value of the key of the row that it stopped on, found on `QueryResult.getLastEvaluatedKey()`.

## Accessing the results
Since the results are returned using a `List<HashMap<String, AttributeValue>>` for each row, they are very easy to access.

The `AttributeValue` type, which is part of the DynamoDB API, has results by type. For this event, you will almost always
call `.withN` to get the result's numerical type value. If you call the wrong type (like `.withS` on a numerical field) there
will be no value to use.

## Retrieve all data from a specific drive scenario
This query demonstrates how you can pull all rows for a drive scenario.
```java
// Drive Scenario Query
private static void queryDriveScenario(final String driveScenarioId) {
        List<Map<String, AttributeValue>> results = new ArrayList<Map<String, AttributeValue>>();
        QueryRequest queryRequest = new QueryRequest()
                .withTableName(TABLE_NAME)
                .withExpressionAttributeValues(
                        new HashMap<String, AttributeValue>() {{
                            put(":dId", new AttributeValue().withN(driveScenarioId));
                        }})
                .withKeyConditionExpression("driveid = :dId");

        QueryResult queryResult;
        do {
            queryResult = client.query(queryRequest);
            results.addAll(queryResult.getItems());
            queryRequest.setExclusiveStartKey(queryResult.getLastEvaluatedKey());
        } while(queryResult.getLastEvaluatedKey() != null && queryResult.getLastEvaluatedKey().size() != 0);
        System.out.println("Retrieved " + results.size() + " logs for Drive Scenario ID " + driveScenarioId);

    }
```

## Retrieve all data at a specific timestamp (`logtime`)
This query demonstrates how to pull all rows for a drive scenario at a specific timestamp. In the event data, this should
always return one (1) row.
```java
private static void querySpecificTimestamp(final String driveScenarioId, final String timestamp) {
    List<Map<String, AttributeValue>> results = new ArrayList<Map<String, AttributeValue>>();
    QueryRequest queryRequest = new QueryRequest()
            .withTableName(TABLE_NAME)
            .withExpressionAttributeValues(
                    new HashMap<String, AttributeValue>() {{
                        put(":dId", new AttributeValue().withN(driveScenarioId));
                        put(":t", new AttributeValue().withN(timestamp));
                    }}
            )
            .withKeyConditionExpression("driveid = :dId and logtime = :t");

    QueryResult queryResult;
    do {
        queryResult = client.query(queryRequest);
        results.addAll(queryResult.getItems());
        queryRequest.setExclusiveStartKey(queryResult.getLastEvaluatedKey());
    } while(queryResult.getLastEvaluatedKey() != null && queryResult.getLastEvaluatedKey().size() != 0);
    System.out.println("Retrieved " + results.size() + " logs at timestamp " + timestamp
            + " with Drive Scenario ID " + driveScenarioId);

}
```

## Retrieve all GPS data for a drive scenario
This query demonstrates how to pull all GPS data for a drive scenario. A `ProjectionExpression` is used to only retrieve
the `GPS_Lat`, `GPS_Lon`, and `GPS_Alt` columns.
```java
List<Map<String, AttributeValue>> results = new ArrayList<Map<String, AttributeValue>>();
QueryRequest queryRequest = new QueryRequest()
        .withTableName(TABLE_NAME)
        .withExpressionAttributeValues(
                new HashMap<String, AttributeValue>() {{
                    put(":dId", new AttributeValue().withN(driveScenarioId));
                }}
        )
        .withProjectionExpression("GPS_Lat, GPS_Lon, GPS_Alt")
        .withKeyConditionExpression("driveid = :dId");

QueryResult queryResult;
do {
    queryResult = client.query(queryRequest);
    results.addAll(queryResult.getItems());
    queryRequest.setExclusiveStartKey(queryResult.getLastEvaluatedKey());
} while(queryResult.getLastEvaluatedKey() != null && queryResult.getLastEvaluatedKey().size() != 0);
System.out.println("Retrieved " + results.size() + " GPS logs for Drive Scenario ID " + driveScenarioId);
```

## Further information
If you need more advanced functionality for your application, you are going to want these. **MAKE SURE THAT YOU USE
DOCUMENTATION THAT IS BOTH UP TO DATE AND FOR API 1.11**
https://docs.aws.amazon.com/amazondynamodb/latest/developerguide/Introduction.html

https://docs.aws.amazon.com/amazondynamodb/latest/developerguide/GettingStarted.NET.html

Good luck!