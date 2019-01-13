# .NET Core C# Code Example - S3

This covers basic AWS S3 interactions using .NET Core. It also applies to
any later versions of .NET that support `async`/`await`. .NET Core _requires_ 
`async`/`await` use or synchronous `.Wait()`. Standard .NET can use the
synchronous API for everything without any type of waits, though it's
recommended to use asynchronous.

Basic interactions are retrieving a file from the S3 bucket and writing/saving
that file locally.

**The goal of this guide is to provide enough information for you to access the
data without having to spend time on a deep-dive of the official documentation.**

## Requirements

1. Visual Studio 2017 Community from
   https://visualstudio.microsoft.com/downloads/
2. AWSSDK.S3 package from NuGet in your Visual Studio project
   `Install-Package AWSSDK.S3`
3. An AWS Access Key, Secret Key, and Region.

## Working with S3

All examples are split between two files in the same partial class 'Program'.

-   **S3ExampleMain.cs** contains the main method and the S3 client creation
    method.
-   **SampleInteractions.cs** contains methods for various interactions with S3
    and their supporting methods.

**This section is a walkthrough of that code. To view the code directly, simply
go to the two files in this folder.**

Here are the imported libraries required for S3:

```csharp
using System;
using System.Threading;
using System.Threading.Tasks;
using Amazon.S3.Model;
```

Declare a S3 client object and the name of the bucket you want to use:

```csharp
class Program
{
    private static AmazonDynamoDBClient client;
    private static readonly string bucketName = "name of bucket";
```

The bucket to use is `p3na-18gus.3101.027`.

### Creating the S3 client to access the S3 API

Before any interactions, the S3 client object must be created so that the
S3 API can be accessed. In the example this is done at the start of the
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
            client = new AmazonS3Client(accessKey, secretKey, region);
        }
        catch (Exception ex)
        {
            Console.WriteLine("     FAILED to create a S3 client; " + ex.Message);
        }
    }
```

> Note that in a production environment, the access key and secret key should
> not be hardcoded in your code files.
> You don't necessarily need to worry about this for this event. If you're
> interested though, here's more information on it:
> https://docs.aws.amazon.com/sdk-for-net/v2/developer-guide/net-dg-config-creds.html

### Retrieving a file

Each file on S3 is stored in a `bucket`. The bucket is similar to web address.

> For all purposes in getting drive scenario-related files, use the bucket
> `p3na-18gus.3101.027`.

In S3, a file is known as an `object`. Each object has a key that is used to find it
within the bucket. The key is a combination of the path to the file within the bucket
and the name of the file.

For example, if object `therockvsdwaynejohnson.mp4` was stored on the path
`stop/hitting/yourself/`, the key would be
`stop/hitting/yourself/therockvsdwaynejohnson.mp4`.

The first step in retrieving a file is to create a `GetObjectRequest` with the
bucket and key.

Here's the structure of creating the request:

```csharp
        private static async Task<GetObjectResponse> RetrieveFile(string objectKey)
        {
            // build the get object request
            var getObjectRequest = new GetObjectRequest()
            {
                BucketName = bucketName,
                Key = objectKey
            };
            // asynchronously wait for GetObject to run and store the response
            var getObjectResponse = await GetObject_async(getObjectRequest);

            return getObjectResponse;
        }
```

### Running a GetObjectRequest

Next we'll need a helper method to handle the request. This method receives a
`GetObjectRequest`, runs it, and returns the `GetObjectResponse` object.

```csharp
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
```

### Saving the file locally

The response object, `GetObjectResponse`, has a couple uses that are detailed here:
https://docs.aws.amazon.com/sdkfornet1/latest/apidocs/html/T_Amazon_S3_Model_GetObjectResponse.htm

The main use is for writing/saving the file locally.

Once we have the `GetObjectResponse`, this is very easy. Create a method to do this:

```csharp
        private static async Task<bool> SaveObject_async(GetObjectResponse getObjectResponse, string saveFilePath)
        {
            // dummy token to work with API. A structured project would handle this at a manageable place and pass it in to this method.
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

and call it while passing in the `GetObjectResponse` from the object retrieval and the location
you want to save it to:

```csharp
        private static async Task<GetObjectResponse> DownloadFile(string objectKey, string saveFilePath)
        {
            // run SaveObject, which saves the object on this computer
            await SaveObject_async(getObjectResponse, saveFilePath);
        }
```

## Sample retrieval and save

In the example code, the sample interaction is called from the main method:

```csharp
        static void Main(string[] args)
        {
            CreateClient();

            string objectPath = "video-files/";
            string objectName = "Recfile P3 Edge 20181120 104743 Webcam Driver Outputiplimage.m4v";
            // key for the object location within the bucket
            string objectKey = objectPath + objectName;
            // local path to save the downloaded file to.
            string saveFilePath = "C:/Users/<username>/Downloads/" + objectName;

            var downloadFileResult = DownloadFile(objectKey, saveFilePath).GetAwaiter().GetResult();
        }
```

Which calls the `DownloadFile()` method, which in turn calls the methods that 
were detailed above:

```csharp
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
```

### S3 bucket structure

The S3 bucket, available here:
http://p3na-18gus.3101.027.s3-website-us-east-1.amazonaws.com/?prefix=
has 4 folders in it.

**Data (.CSV) files**: The drive scenario .CSV file, which contains all of the
information found on the DynamoDB table, is under the `data-files/` folder with
name `database.csv`.

**Raw log (.REC) files**: `rec-files/`. tbd.

**Sample files**: `sample-files/`. tbd.

**Video (.M4V) files**: The drive scenario camera recordings, both cabin (driver)
and front-facing are under the `video-files/` folder. Cabin videos are named

`Recfile P3 Edge [drive id date] [drive id time] Webcam Driver Outputiplimage.m4v`

(example: Recfile P3 Edge 20181120 104743 Webcam Driver Outputiplimage.m4v).

Front-facing videos are named

`Recfile P3 Edge [drive id date] [drive id time] Webcam Logitech Forward Outputiplimage.m4v`

(example: Recfile P3 Edge 20181120 104743 Webcam Logitech Forward Outputiplimage.m4v).

## Further information

https://docs.aws.amazon.com/AmazonS3/latest/dev/RetrievingObjectUsingNetSDK.html
