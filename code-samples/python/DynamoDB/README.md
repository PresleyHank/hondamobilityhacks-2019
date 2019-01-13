# Python DynamoDB Code Example

## Requirements
1. Python 3 (Recommended version 3.7.2+) 
2. Python Pip
3. Boto 3 Libary for working with AWS. [https://boto3.amazonaws.com/v1/documentation/api/latest/index.html]
4. An AWS Access Key, Secret Key, and Region.  These will be distributed from the staff at the event.

## Installation
Working with the Boto3 library within Python is very simple to get installed.  After the installation of Python
and pip, simply execute the command `pip install boto3`.  Pip should take care of the installation process.

## Working with Boto3
All examples are split between two files in the same partial class 'Program'.
- **Program.cs** contains the main method and the DynamoDB client creation 
method.
- **SampleQueries.cs** contains methods for various queries and their supporting 
methods.

**This section is a walkthrough of that code. To view the code directly, simply 
go to the two files in this folder.**

Here are the imported libraries required for AWS:

```python
    import boto3
```

And required for the example code, but not for AWS:

```python
    import json
    import decimal
    from pathlib import Path
    from boto3.dynamodb.conditions import Key, Attr
    from decimal import Decimal
```

Declare a DynamoDB client object and the name of the table you want to use:

```python
    dynamodb_obj = boto3.resource('dynamodb')
    table = dynamodb_obj.Table(<table-name>)
```

### Creating the DynamoDB client to access the DynamoDB API

Before any queries, the DynamoDB client object must be created so that the 
DynamoDB API can be accessed. We must also pass our access and secret key to the client.  Additionally,
are able to select the region here.

```python
    client = boto3.client(
        'dynamodb',
        aws_access_key_id='',
        aws_secret_access_key='',
        region_name='us-east-1'
    )
```

> Note that in a production environment, the access key and secret key should 
not be hardcoded in your code files.
> You don't necessarily need to worry about this for this event. If you're 
interested though, here's more information on it:
> https://boto3.amazonaws.com/v1/documentation/api/latest/guide/quickstart.html#configuration

### Creating a QueryRequest

A `QueryRequest` is the object that represents your query. Each one can be 
customized depending on the information you're trying to fetch from the 
database.

Some additional import statements are required for the example below:
```python
    from boto3.dynamodb.conditions import Key, Attr
```
In the examples, each `QueryRequest` is executed as a dot operation on the table object we create.

```python
    # Define our table name within DynamodDB
    table = dynamodb.Table('TableName')

    response = table.query(
        ProjectionExpression="column1, column2, column3",
        KeyConditionExpression=Key('keyname').eq(keyname_variable)
    )
```

Notes on Query Parameters: 
>**ProjectionExpression**: specifies the attributes you want in the scan result.

>**FilterExpression**: specifies a condition that returns only items that satisfy the condition. 
>All other items are discarded.

>**ExpressionAttributeNames**: provides name substitution. We use this because year is a reserved word in DynamoDB—you can't
> use it directly in any expression, including KeyConditionExpression. You can use the expression attribute name #yr to 
> address this.

>**ExpressionAttributeValues**: provides value substitution. You use this because you can't use literals in any expression, 
>including KeyConditionExpression. You can use the expression attribute value :yyyy to address this.
>Here's the structure of creating the request:

Every query must act upon an existing table object.  If we want to query separate tables, 
we must execute two distinct queries.

```python
table1 = dynamodb.Table('TableName1')
table2 = dynamodb.Table('TableName2')

response1 = table1.query(
    ProjectionExpression="column1, column2, column3",
    KeyConditionExpression=Key('keyname').eq(keyname_variable)
)

response2 = table2.query(
    ProjectionExpression="column4, column5, column6",
    KeyConditionExpression=Key('keyname').eq(keyname_variable)
)  
```

> DynamoDB works slightly differently than MySQL, MSSQL, or other popular 
databases that you might be used to. Because it is a distributed cloud service 
on AWS, there is no direct IP address or single storage place. Each table has 
a composite primary key that is defined by two sub keys. The first is the 
**partition key** which tells DynamoDB that rows are heavily related and to try 
to store them together. In this data, `driveid` is the partition key (and 
represents the drive scenario). The next part of the primary key is the 
**sort key** which is the default sorting used on the rows. **Every query must
use an equals `=` expression with the `partition key` (`driveid`). This 
means that any single query can only pull records from one drive scenario.

A `FilterExpression` is an **optional** filter that can be applied to filter any 
column:

```python
    FilterExpression=Key('logtime').between(1376395,1476595)
```
Finally, to select which columns you want to retrieve for each row, you use a 
`ProjectionExpression`:

```python
    ProjectionExpression = "Columns, you, want, to, retrieve"
```

If no `ProjectionExpression` is given, all columns will be returned.

### Running a QueryRequest

Next we'll need a helper method to handle queries. This method receives a 
`QueryRequest`, runs it, and returns the compiled results.

```python
TODO
```

Note this loop:

```python
TODO
```

Queries to DynamoDB are limited to a certain size, measured in bytes. If you 
pull all the columns from the table, the size limit is hit at about 400 rows. 
To work around this, if a query hits the limit before all the results are 
returned, a `LastEvaluatedKey` attribute will have a value of the key of the 
row that it stopped on. You can take this key and assign it to the 
`ExclusiveStartKey` value of the same query request, run the query again, and 
it will start at that key.

On each 'sub' query, the results are compiled in a 'master' results list and 
then returned once the entire query is finished:

```python
TODO
```

### Accessing the results

Since the results are returned using a dictionary, with Items in a key, we must access each item
using:
```python
# Get the `logtime` attribute value of the first row
response['Items']['logtime'][0]
```

## Sample queries

In the example code, all sample queries are called from the main method and then 
some attribute values are printed off:

```python
if __name__ == '__main__':
    drive_scenario_id = 20181120104743
    sample_timestamp = 1376395

    # Done
    drive_scenario_query_result = query_drive_scenario(drive_scenario_id)
    timestamp_query_result = query_specific_timestamp(drive_scenario_id, sample_timestamp)
    gps_data_only_results = query_all_gps_data(drive_scenario_id)

    print_log_time(drive_scenario_query_result['Items'][0])
    print_log_time(timestamp_query_result['Items'][0])
    print_gps_data(gps_data_only_results['Items'][0])
```

### Retrieve all data from a specific drive scenario

This query demonstrates how to pull all rows for a drive scenario.

```python
def query_drive_scenario(driveid):
    qdsreponse = table.query(
        KeyConditionExpression=Key('driveid').eq(driveid)
    )
    if 'LastEvaluatedKey' in qdsreponse:
        qds_cont = True
        qds_cont_key = qdsreponse['LastEvaluatedKey']
    else:
        qds_cont = False
    while qds_cont:
        qdsreponse2 = None
        qdsreponse2 = table.query(
            KeyConditionExpression=Key('driveid').eq(driveid),
            ExclusiveStartKey=qds_cont_key
        )
        qdsreponse['Items'].extend(qdsreponse2['Items'])
        if 'LastEvaluatedKey' in qdsreponse2:
            qds_cont = True
            qds_cont_key = None
            qds_cont_key = qdsreponse2['LastEvaluatedKey']
        else:
            qds_cont = False
    print_length(qdsreponse)
    return qdsreponse
```

### Retrieve all data at a specific timestamp (`logtime`)

This query demonstrates how to pull all rows for a drive scenario at a specific 
timestamp. In the event data, this should always return only one row.

```python
def query_specific_timestamp(driveid, timestamp):
    qstreponse = table.query(
        KeyConditionExpression=Key('driveid').eq(driveid) & Key('logtime').eq(timestamp)
    )
    print_length(qstreponse)
    return qstreponse
```

### Retrieve all GPS data for a drive scenario

This query demonstrates how to pull all GPS data for a drive scenario. A 
`ProjectionExpression` is used to only retrieve the `GPS_Lat`, `GPS_Lon`, and 
`GPS_Alt` columns

```python
def query_all_gps_data(driveid):
    response = table.query(
        ProjectionExpression="GPS_Lat, GPS_Lon, GPS_Alt, logtime",
        KeyConditionExpression=Key('driveid').eq(driveid)
    )
    if 'LastEvaluatedKey' in response:
        cont = True
        cont_key = response['LastEvaluatedKey']
    else:
        cont = False
    while cont:
        reponse2 = None
        reponse2 = table.query(
            KeyConditionExpression=Key('driveid').eq(driveid),
            ExclusiveStartKey=cont_key
        )
        response['Items'].extend(reponse2['Items'])
        if 'LastEvaluatedKey' in reponse2:
            cont = True
            cont_key = None
            cont_key = reponse2['LastEvaluatedKey']
        else:
            cont = False
    print_length(response)
    return response
```

### GPS printing

Here's the method that checks if the results contain GPS data and then prints:

```python
def print_gps_data(single_log):
    gps_lat = str(single_log['GPS_Lat'])
    gps_lon = str(single_log['GPS_Lon'])
    gps_alt = str(single_log['GPS_Alt'])
    if gps_lat and gps_lon and gps_alt:
        print("Lat: " + gps_lat + ", Lon: " + gps_lon + ", Alt: " + gps_alt)
    else:
        print("Log does not contain valid GPS data.")
```

## Further information

https://docs.aws.amazon.com/amazondynamodb/latest/developerguide/Introduction.html

https://docs.aws.amazon.com/amazondynamodb/latest/developerguide/GettingStarted.Python.html

https://boto3.amazonaws.com/v1/documentation/api/latest/index.html