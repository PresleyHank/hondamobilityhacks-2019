# Javascript Code Example
This covers basic AWS DynamoDB interactions using Javascript. **The goal of this guide is to provide enough information for you
to access the data without having to spend time on a deep-dive of the official documentation.**

## Requirements 
1. Your favourite Javascript IDE (Visual Studio Code)
2. Javascript AWS SDK
3. An AWS Access key, Secret key, and Region

## AWS SDK Setup
To use the AWS SDK, simply include this in your code and reference it:
```javascript
var AWS = require("aws-sdk");

AWS.config.update({
  region: "us-east-2",
  endpoint: "https://dynamodb.us-east-2.amazonaws.com",
  accessKeyId: "YOUR_ACCESS_KEY",
  secretAccessKey: "YOUR_SECRET_KEY"
});
```

This will automatically import the SDK from your `node_modules` folder.

## Working with DynamoDB and creating your client
In our examples, we have three different queries in three different `.js` files
* 1 - Get Data with Timestamp.js
* 2 - Query Drive Scenario.js
* 3 - Query with GPS Filter.js

In order to connect to the database, you need to attach a client variable to the AWS config you setup earlier:
```javascript
var AWS = require("aws-sdk");

AWS.config.update({
  region: "us-east-1",
  endpoint: "https://dynamodb.us-east-1.amazonaws.com",
  accessKeyId: "YOUR_ACCESS_KEY",
  secretAccessKey: "YOUR_SECRET_KEY"
});

var client = new AWS.DynamoDB.DocumentClient();
```
From there, you are able to use calls such as `client.query(params, function(err,data){});` to interact with the data.

## Creating a Query
Each query can be customized depending on the information that you're trying to fetch from the database. In the examples, each `client.query(...);` call uses different `params` to determine what data to fetch.

Each query must have a defined table name, otherwise it will return the entire database. Any values that are used must be given a name and a type:
```javascript
var params = {
  TableName: tablename,
  ExpressionAttributeValues: { ":dId" : driveScenarioId },
  KeyConditionExpression: "driveid = :dId"
};
```

**:dId** can be any name you want, however for `driveID` it is easiest to just use the above code.

Numbers are used for all numerical fields. This covers almost all of the data for this event. For the few string fields, use strings instead. **NOTE:** unlike in the API for other languages such as Java or C#, the `N:` or `S:` value of the field is inferred from the type of variable you are passing into the parameters.

The basic (and required) filter is on the partition key. This must be included in every query:
```javascript
var params = {
    TableName: tablename,
    KeyConditionExpression: "driveid = :dId"
};
```

> DynamoDB works slightly differently than MySQL, MSSQL, or other popular databases that you may be used to. Because it
is a distributed cloud service on AWS, there is no direct IP address or single storage place. Each table has a composite
primary key that is defined by two sub keys. The first is the **partition key** which tells DynamoDB that rows are heavily
related and to try to store them together. In this data, `driveid` is the partition key (and represents the drive scenario).
The next part of the primary key is the **sort key** which is the default sorting used on the rows. **Every query must
use an equals `=` expression with the `partition key` (`driveid = :dId`). This means that any single query can only pull
records from one drive scenario.

A `FilterExpression` is an **optional** filter that can be applied to filter any column:
```javascript
var params = {
    TableName: tablename,
    KeyConditionExpression: "driveid = :dId",
    FilterExpression: "ColumnValue > 10"
};
```

Finally, to select which column you want to retrieve for each row, use a `ProjectionExpression`:
```javascript
var params = {
    TableName: tablename,
    KeyConditionExpression: "driveid = :dId",
    ProjectionExpression: "GPS_Lat, GPS_Lon, GPS_Alt"
};
```
If no `ProjectionExpression` is given, all columns will be returned.

Queries to DynamoDB are limited to a certain size, measured in bytes. If you pull all the columns from the table, the size limit is hit at about 400 rows. To work around this, if a query hits the limit before all the results are returned, a `LastEvaluatedKey` attribute will have a value of the key of the row that it stopped on.

## Accessing the Results
Since the results are returned in a callback function, they are very easy to access.
```javascript
docClient.query(params, onScan);
var count = 0;

function onScan(err, data) {
  if(err) {
    console.error("Unable to query the table. Error JSON: ", JSON.stringify(err, null, 2));
  } else {
    console.log("Query succeeded");
    count += data.Items.length;

    // continue querying if we have more items
    if(typeof data.LastEvaluatedKey != "undefined") {
      console.log("Querying for more...");
      params.ExclusiveStartKey = data.LastEvaluatedKey;
      docClient.query(params, onScan);
    }
    console.log(`Found ${count} items`);
  }
}
```
First we scan the database for the parameters, then we run our function that scans the data for exactly what we are looking for. The data is returned via `data.Items` that you can loop through however you would like.

## Further Information
If you need more advanced functionality for your application, you are going to want these:
https://docs.aws.amazon.com/amazondynamodb/latest/developerguide/Introduction.html

https://docs.aws.amazon.com/amazondynamodb/latest/developerguide/GettingStarted.NET.html