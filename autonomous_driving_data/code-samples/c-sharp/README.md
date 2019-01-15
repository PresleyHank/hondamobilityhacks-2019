# .NET Core C# Code Example

This covers basic AWS DynamoDB and S3 interactions using .NET Core. It also applies to
any later versions of .NET that support `async`/`await`. .NET Core _requires_ 
`async`/`await` use or synchronous `.Wait()`. Standard .NET can use the
synchronous API for everything without any type of waits, though it's
recommended to use asynchronous.

**The goal of this guide is to provide enough information for you to access the
data without having to spend time on a deep-dive of the official documentation.**

All .NET DynamoDB and S3 sample code and documentation is in their respective folder.

# TL;DR

Below is a TL;DR version of both querying DynamoDB and retrieving a file from 
S3. (Yes, it is still quite long.)

**For more in-depth documentation, see the READMEs in the DynamoDB and S3 folders.**

TL;DR links:

1. [DynamoDB](#dynamodb)
2. [S3](#s3)

## DynamoDB

1. Add the required library to your project. NuGet: `Install-Package AWSSDK.DynamoDBv2` or .NET: `dotnet add package AWSSDK.DynamoDBv2`.
2. Get an Access Key and Secret Key from hackathon organizers. Use region `USEast1`.
3. Import the required libraries.
```csharp
using System;
using System.Text;
using System.Threading.Tasks;
using System.Collections.Generic;
using Amazon.DynamoDBv2;
using Amazon.DynamoDBv2.Model;
```
4. Create the DynamoDB client. Assign to higher/static scope `AmazonDynamoDBClient client` variable.
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
5. Create and call the query request, then print out some of the results.
```csharp
var gpsDataOnlyResults = QueryAllGpsData("20181127151525").GetAwaiter().GetResult();

// ...

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

// ...

private static async Task<List<Dictionary<string, AttributeValue>>> ClientQuerying_async(QueryRequest qRequest)
{
    // since queries have a size limit, we need to append the results of each one to a 'master' list
    var results = new List<Dictionary<string, AttributeValue>> { };

    QueryResponse qResponse;

    do
    {
        try
        {
            // run the task
            qResponse = await client.QueryAsync(qRequest);
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

    return results;
}

// ...

PrintGpsData(gpsDataOnlyResults[0]);

// ...

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

## S3

1. Add the required library to your project. NuGet: `Install-Package AWSSDK.S3` or .NET: `dotnet add package AWSSDK.S3`.
2. Get an Access Key and Secret Key from hackathon organizers. Use region `USEast1`.
3. Import the required libraries.
```csharp
using System;
using System.Threading;
using System.Threading.Tasks;
using Amazon.S3;
using Amazon.S3.Model;
```
4. Create the S3 client. Assign to higher/static scope `AmazonS3Client client` variable.
```csharp
private static void CreateClient()
{
    string accessKey = "********************";
    string secretKey = "****************************************";
    var region = Amazon.RegionEndpoint.USEast1;

    try
    {
        client = new AmazonS3Client(accessKey, secretKey, region);
    }
    catch (Exception ex)
    {
        Console.WriteLine("     FAILED to create a S3 client; " + ex.Message);
    }
}
```
5. Create and call the get object request, then write the file locally.
```csharp
string objectPath = "video-files/";
string objectName = "Recfile P3 Edge 20181120 104743 Webcam Driver Outputiplimage.m4v";
// key for the object location within the bucket
string objectKey = objectPath + objectName;
// local path to save the downloaded file to.
string saveFilePath = "local/file/path/" + objectName;

DownloadFile(objectKey, saveFilePath).GetAwaiter().GetResult();

// ...

private static async Task<GetObjectResponse> DownloadFile(string objectKey, string saveFilePath)
{
    // build the get object request
    var getObjectRequest = new GetObjectRequest()
    {
        BucketName = bucketName,
        Key = objectKey
    };
    // run GetObject and store the response
    var getObjectResponse = await GetObject_async(getObjectRequest);
    // run SaveObject, which saves the object on this computer
    await SaveObject_async(getObjectResponse, saveFilePath);

    return getObjectResponse;
}

// ...

private static async Task<GetObjectResponse> GetObject_async(GetObjectRequest getObjectRequest)
{
    GetObjectResponse getObjectResponse;

    try
    {
        // run the task
        getObjectResponse = await client.GetObjectAsync(getObjectRequest);
    }
    catch (Exception ex)
    {
        Console.WriteLine("      The get object FAILED, because:\n       {0}.", ex.Message);
        return new GetObjectResponse();
    }

    Console.WriteLine("     -- Retrieved object with key {0} from bucket {1}.",
        getObjectResponse.Key, getObjectResponse.BucketName);

    return getObjectResponse;
}

// ...

private static async Task<bool> SaveObject_async(GetObjectResponse getObjectResponse, string saveFilePath)
{
    var source = new CancellationTokenSource();
    var token = source.Token;

    try
    {
        await getObjectResponse.WriteResponseStreamToFileAsync(saveFilePath, false, token);
    }
    catch (Exception ex)
    {
        Console.WriteLine("      The save object FAILED, because:\n       {0}.", ex.Message);
        return false;
    }

    return true;
}
```