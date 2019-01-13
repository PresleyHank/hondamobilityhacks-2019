# .NET Core C# Code Example - DynamoDB

This covers basic AWS DynamoDB interactions using .NET Core. It also applies to
any later versions of .NET that support `async`/`await`. .NET Core _requires_ 
`async`/`await` use or synchronous `.Wait()`. Standard .NET can use the
synchronous API for everything without any type of waits, though it's
recommended to use asynchronous.

**The goal of this guide is to provide enough information for you to access the
data without having to spend time on a deep-dive of the official documentation.**

## Requirements

1. Visual Studio 2017 Community from
   https://visualstudio.microsoft.com/downloads/
2. AWSSDK.DynamoDBv2 package from NuGet in your Visual Studio project
   `Install-Package AWSSDK.DynamoDBv2`
3. An AWS Access Key, Secret Key, and Region.

## Working with DynamoDB

All examples are split between two files in the same partial class 'Program'.

-   **DynamoDBExampleMain.cs** contains the main method and the DynamoDB client creation
    method.
-   **SampleQueries.cs** contains methods for various queries and their supporting
    methods.

**This section is a walkthrough of that code. To view the code directly, simply
go to the two files in this folder.**

Here are the imported libraries required for DynamoDB:

```csharp
using System;
using System.Threading.Tasks;
using System.Collections.Generic;
using Amazon.DynamoDBv2.Model;
```

And required for the example code, but not for DynamoDB:

```csharp
using System.Text;
```

Declare a DynamoDB client object and the name of the table you want to use:

```csharp
class Program
{
    private static AmazonDynamoDBClient client;
    private static readonly string tableName = "name of table";
```

The table name to use is `honda-hackathon1`.

### Creating the DynamoDB client to access the DynamoDB API

Before any queries, the DynamoDB client object must be created so that the
DynamoDB API can be accessed. In the example this is done at the start of the
`Main` method:

```csharp
static void Main(string[] args)
{
    CreateClient();
```

Which calls this method:

```csharp
    private static void CreateClient()
    {
        string accessKey = "********************";
        string secretKey = "****************************************";
        var region = Amazon.RegionEndpoint.USEast1;

        try
        {
            client = new AmazonDynamoDBClient(accessKey, secretKey, region);
        }
        catch (Exception ex)
        {
            Console.WriteLine("     FAILED to create a DynamoDB client; " + ex.Message);
        }
    }
```

> Note that in a production environment, the access key and secret key should
> not be hardcoded in your code files.
> You don't necessarily need to worry about this for this event. If you're
> interested though, here's more information on it:
> https://docs.aws.amazon.com/sdk-for-net/v2/developer-guide/net-dg-config-creds.html

### Creating a QueryRequest

A `QueryRequest` is the object that represents your query. Each one can be
customized depending on the information you're trying to fetch from the
database.

In the examples, each `QueryRequest` is defined in its own method for
readability. If you are dealing with many unique queries, it might be a good
idea to create a method that handles building these.

Here's the structure of creating the request:

```csharp
        public static async Task<List<Dictionary<string, AttributeValue>>> QuerySomething(string driveID)
        {
            var qRequest = new QueryRequest
            {
                TableName = tableName,
                ExpressionAttributeValues = new Dictionary<string, AttributeValue>
                {
                    { ":dId", new AttributeValue { N = driveID } }
                },
                KeyConditionExpression = "driveid = :dId",
                FilterExpression = "ColumnValue > 10",
                ProjectionExpression = "Columns, you, want, to, retrieve"
            };
        }
```

Every query must define the table it is querying (which we defined on the class
level):

```csharp
                TableName = tableName
```

Any values that are used have to be given a name and a type:

```csharp
                ExpressionAttributeValues = new Dictionary<string, AttributeValue>
                {
                    { ":dId", new AttributeValue { N = driveID } }
                }
```

**dId** can be any name you want, however for `driveID` it's easiest to just use
the above code.

The `N` is used for all numerical fields. This covers almost all of the data for
this event. For the few string fields, use `S` instead.

The basic (and required) filter is on the partition key. This must be included
in every query:

```csharp
                KeyConditionExpression = "driveid = :dId"
```

> DynamoDB works slightly differently than MySQL, MSSQL, or other popular
> databases that you might be used to. Because it is a distributed cloud service
> on AWS, there is no direct IP address or single storage place. Each table has
> a composite primary key that is defined by two sub keys. The first is the
> **partition key** which tells DynamoDB that rows are heavily related and to try
> to store them together. In this data, `driveid` is the partition key (and
> represents the drive scenario). The next part of the primary key is the
> **sort key** which is the default sorting used on the rows. \*\*Every query must
> use an equals `=` expression with the `partition key` (`driveid = :dId`). This
> means that any single query can only pull records from one drive scenario.

A `FilterExpression` is an **optional** filter that can be applied to filter any
column:

```csharp
                FilterExpression = "ColumnValue > 10"
```

Finally, to select which columns you want to retrieve for each row, you use a
`ProjectionExpression`:

```csharp
                ProjectionExpression = "Columns, you, want, to, retrieve"
```

If no `ProjectionExpression` is given, all columns will be returned.

### Running a QueryRequest

Next we'll need a helper method to handle queries. This method receives a
`QueryRequest`, runs it, and returns the compiled results.

```csharp
    private static async Task<List<Dictionary<string, AttributeValue>>> ClientQuerying_async(QueryRequest qRequest)
    {
        // since queries have a size limit, we need to append the results of each one to a 'master' list
        var results = new List<Dictionary<string, AttributeValue>> { };

        QueryResponse qResponse;

        do
        {
            try
            {
                // build the query task
                var clientQueryTask = client.QueryAsync(qRequest);
                // run the task
                qResponse = await clientQueryTask;
                // add the results of the size-limited query to the overall results
                results.AddRange(qResponse.Items);
                // add the last evaluated key to the next query so it knows where to start
                qRequest.ExclusiveStartKey = qResponse.LastEvaluatedKey;
            }
            catch (Exception ex)
            {
                Console.WriteLine("      The low-level query FAILED, because:\n       {0}.", ex.Message);
                return new List<Dictionary<string, AttributeValue>>();
            }
        } while (qResponse.LastEvaluatedKey != null && qResponse.LastEvaluatedKey.Count != 0);

        Console.WriteLine("     -- Retrieved {0} logs.", results.Count);

        return results;
    }
```

Note this loop:

```csharp
        do
        {
            // ...
                qRequest.ExclusiveStartKey = qResponse.LastEvaluatedKey;
            // ...
        } while (qResponse.LastEvaluatedKey != null && qResponse.LastEvaluatedKey.Count != 0);
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

```csharp
        var results = new List<Dictionary<string, AttributeValue>> { };

        do
        {
            // ...
                results.AddRange(qResponse.Items);
            // ...
        } while ()

        return results;
```

### Accessing the results

Since the results are returned using a
`List<Dictionary<string, AttributeValue>>` for each row, they are very easy to
access:

```csharp
// get the `logtime` attribute value of the first row
results[0]["logtime"].N
```

The `AttributeValue` type, which is part of the DynamoDB library, has results
by type. For this event, you will almost always call `.N` to get the result's
numerical type value. If you call the wrong type (like `.S` on a numerical
field) there will be no value to use.

Here's an example that prints out the `logtime`:

```csharp
    static void Main(string[] args)
    {
        // ...

        // query something
        var queryResults = QuerySomething(driveScenarioID).GetAwaiter().GetResult();

        // print the `logtime` of the first row of the results
        PrintLogTime(queryResults[0]);
    }

    private static void PrintLogTime(Dictionary<string, AttributeValue> log)
    {
        string tsAttr = "logtime";

        if (log.ContainsKey(tsAttr))
        {
            Console.WriteLine(log[tsAttr].N);
        }
        else
        {
            Console.WriteLine("Log does not contain the \"{0}\" attribute.", tsAttr);
        }
    }
```

## Sample queries

In the example code, all sample queries are called from the main method and then
some attribute values are printed off:

```csharp
    static void Main(string[] args)
    {
        CreateClient();

        string driveScenarioID = "20181218091707";

        var driveScenarioQueryResults = QueryDriveScenario(driveScenarioID).GetAwaiter().GetResult();
        var timestampQueryResults = QuerySpecificTimestamp(driveScenarioID, "1365385").GetAwaiter().GetResult();
        var gpsDataOnlyResults = QueryAllGpsData(driveScenarioID).GetAwaiter().GetResult();

        PrintLogTime(driveScenarioQueryResults[0]);
        PrintLogTime(timestampQueryResults[0]);
        PrintGpsData(gpsDataOnlyResults[0]);
    }
```

### Retrieve all data from a specific drive scenario

This query demonstrates how to pull all rows for a drive scenario.

```csharp
    public static async Task<List<Dictionary<string, AttributeValue>>> QueryDriveScenario(string driveID)
    {
        var qRequest = new QueryRequest
        {
            TableName = tableName,
            ExpressionAttributeValues = new Dictionary<string, AttributeValue>
            {
                // set a query value for the drive ID value as a number.
                { ":dId", new AttributeValue { N = driveID } }
            },
            // get all results for the drive ID
            KeyConditionExpression = "driveid = :dId"
        };

        var queryTask = await ClientQuerying_async(qRequest);

        return queryTask;
    }
```

### Retrieve all data at a specific timestamp (`logtime`)

This query demonstrates how to pull all rows for a drive scenario at a specific
timestamp. In the event data, this should always return only one row.

```csharp
    public static async Task<List<Dictionary<string, AttributeValue>>> QuerySpecificTimestamp(string driveID, string time)
    {
        var qRequest = new QueryRequest
        {
            TableName = tableName,
            ExpressionAttributeValues = new Dictionary<string, AttributeValue>
            {
                // set a query value for the drive ID value as a number.
                { ":dId", new AttributeValue { N = driveID } },
                // set a query value for the logtime/timestamp value as a number.
                { ":t", new AttributeValue { N = time } }
            },
            // get all results for the drive ID where the logtime is equal to the timestamp
            KeyConditionExpression = "driveid = :dId and logtime = :t"
        };

        var queryTask = await ClientQuerying_async(qRequest);

        return queryTask;
    }
```

### Retrieve all GPS data for a drive scenario

This query demonstrates how to pull all GPS data for a drive scenario. A
`ProjectionExpression` is used to only retrieve the `GPS_Lat`, `GPS_Lon`, and
`GPS_Alt` columns

```csharp
    public static async Task<List<Dictionary<string, AttributeValue>>> QueryAllGpsData(string driveID)
    {
        var qRequest = new QueryRequest
        {
            TableName = tableName,
            ExpressionAttributeValues = new Dictionary<string, AttributeValue>
            {
                // set a query value for the drive ID value as a number.
                { ":dId", new AttributeValue { N = driveID } }
            },
            // get all results for the drive ID
            KeyConditionExpression = "driveid = :dId",
            // only get the GPS_Lat, GPS_Lon, and GPS_Alt columns
            ProjectionExpression = "GPS_Lat, GPS_Lon, GPS_Alt"
        };

        var queryTask = await ClientQuerying_async(qRequest);

        return queryTask;
    }
```

### GPS printing

Here's the method that checks if the results contain GPS data and then prints:

```csharp
    private static void PrintGpsData(Dictionary<string, AttributeValue> log)
    {
        string latAttr = "GPS_Lat";
        string lonAttr = "GPS_Lon";
        string altAttr = "GPS_Alt";

        var gpsData = new StringBuilder();

        // check for latitude
        if (log.ContainsKey(latAttr))
        {
            gpsData.Append("Lat: " + log[latAttr].N + ", ");
        }
        else
        {
            Console.WriteLine("Log does not contain the \"{0}\" attribute.", latAttr);
        }

        // check for longitude
        if (log.ContainsKey(lonAttr))
        {
            gpsData.Append("Lon: " + log[lonAttr].N + ", ");
        }
        else
        {
            Console.WriteLine("Log does not contain the \"{0}\" attribute.", lonAttr);
        }

        // check for altitude
        if (log.ContainsKey(altAttr))
        {
            gpsData.Append("Alt: " + log[altAttr].N);
        }
        else
        {
            Console.WriteLine("Log does not contain the \"{0}\" attribute.", altAttr);
        }

        Console.WriteLine(gpsData.ToString());
    }
```

## Further information

https://docs.aws.amazon.com/amazondynamodb/latest/developerguide/Introduction.html

https://docs.aws.amazon.com/amazondynamodb/latest/developerguide/GettingStarted.NET.html
