# C++ Code Example 
This covers basic AWS interactions using C++. 

**The goal of this guide is to provide enough information for you to access the 
data without having to spend time on a deep-dive of the official documentation.**

## Requirements
1.  A C++ compiler (one of the following)
    1. Visual Studio 2017 Community from https://visualstudio.microsoft.com/downloads/
    2. The GNU Compiler Collection (GCC)
    3. Clang
2.  The SDK Files (one of the following)
    1. AWSSDKCPP dynamodb package from NuGet in your Visual Studio project
    2. Built from source (source files found at https://github.com/aws/aws-sdk-cpp)
3. An AWS Access Key, Secret Key, and Region
## More information
https://docs.aws.amazon.com/sdk-for-cpp/v1/developer-guide/setup.html

## Working with DynamoDB
All examples are full programs that can be compiled run from the command line
and serve as examples of simple interaction with DynamoDB

**This section is an explanation of that code. To view the code directly, simply 
go to the four files in this folder.**

Here are the imported header files required for our examples:

```c++
#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h> 
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/AttributeDefinition.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
```
In addition depending on your interaction you may also need to include further
headers for particular classes.

```c++
#include <aws/dynamodb/model/GetItemRequest.h>
```
and
```c++
#include <aws/dynamodb/model/QueryRequest.h>
```
Are two such header files that are needed in some of our examples.

Also required for the example code, but not for DynamoDB:
```c++
#include <iostream>
```

Initialize the AWS API before interacting with a client
```c++
Aws::SDKOptions options;
Aws::InitAPI(options);
```
and close the API before exiting the program
```c++
Aws::ShutdownAPI(options);
```

### Creating the DynamoDB client to access the DynamoDB API

Before any queries, the DynamoDB client object must be created so that the 
DynamoDB API can be accessed. In the example, this is done right after 
initializing the AWS API:

```c++
Aws::Client::ClientConfiguration config;
Aws::Auth::AWSCredentials cred;
cred.SetAWSAccessKeyId("**********");
cred.SetAWSSecretKey("**********");

config.region = "us-east-1";

Aws::DynamoDB::DynamoDBClient client(cred, config);
```

> Note that in a production environment, the access key and secret key should 
not be hardcoded in your code files.
> You don't necessarily need to worry about this for this event.

### Creating a ScanRequest
A `ScanRequest` is the object that represents your scan. Scans fetch all 
data from the database. For this reason it requires very minimal setup, 
only needing the name of the table. Note, however, that it can be __very__ 
resource intensive and thus is probably best avoided in most situations.

```c++
Aws::DynamoDB::Model::ScanRequest req;

req.SetTableName("honda-hackathon1");
```

### Creating a GetItemRequest
A `GetItemRequest` is an object that allows you to fetch from a single row from the 
database. Here is how to create a `GetItemRequest` object

```c++
Aws::DynamoDB::Model::GetItemRequest req;

req.SetTableName("honda-hackathon1");

Aws::DynamoDB::Model::AttributeValue timestamp;
timestamp.SetN("1365385");
req.AddKey("logtime", timestamp);

Aws::DynamoDB::Model::AttributeValue drive_id;
drive_id.SetN("20181218091707");
req.AddKey("driveid", drive_id);
```

Every `GetItemRequest` needs to have both its partition key and its sort key 
added in order to uniquely identify a row from the database. In this example, 
`driveid` is the partition key, and `logtime` is the sort key.
As well, a `GetItemRequest` must set the table name of the table it wishes to retrieve from.

> DynamoDB works slightly differently than MySQL, MSSQL, or other popular 
databases that you might be used to. Because it is a distributed cloud service 
on AWS, there is no direct IP address or single storage place. Each table has 
a composite primary key that is defined by two sub keys. The first is the 
**partition key** which tells DynamoDB that rows are heavily related and to try 
to store them together. In this data, `driveid` is the partition key (and 
represents the drive scenario). The next part of the primary key is the 
**sort key** which is the default sorting used on the rows.

### Creating a QueryRequest

A `QueryRequest` is like a more complicated (but also more useful) version
of a `ScanRequest`. Each one can be customized depending on the information
you're trying to fetch from the database.

In the examples, each `QueryRequest` is defined independently in each file
where it is used. If you are dealing with many unique queries, it might be a good 
idea to create a method that handles building these.

Here's the structure of creating the request:

```c++
Aws::DynamoDB::Model::QueryRequest req;

Aws::DynamoDB::Model::AttributeValue drive_id;

Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue> expression_attr;

req.SetTableName(table_name);

drive_id.SetN(drive_value);
expression_attr[":drive_id"] = drive_id;
req.SetExpressionAttributeValues(expression_attr);
req.SetKeyConditionExpression("driveid = :drive_id");
```

Every query must define the table it is querying (which is hardcoded into each of our examples, 
but can also be a variable if desired):


Any values that are used have to be given a name and a type:

```c++
drive_id.SetN(drive_value);
expression_attr[":drive_id"] = drive_id;
req.SetExpressionAttributeValues(expression_attr);
req.SetKeyConditionExpression("driveid = :drive_id");
```

**drive_id** can be any name you want, however for `driveid` it's easiest to just use 
the above code.

The `SetN()` method is used for all numerical fields. This covers almost all of the data for 
this event. For the few string fields, use `SetS()` instead.

The basic (and required) filter is on the partition key. This must be included 
in every query:

```c++
expression_attr[":drive_id"] = drive_id;
req.SetExpressionAttributeValues(expression_attr);
req.SetKeyConditionExpression("driveid = :drive_id");
```

> *Every query must use an equals `=` expression with the `partition key` (`driveid = :drive_id`). This means that any single query can only pull records from one drive scenario.

A `FilterExpression` is an **optional** filter that can be applied to filter any 
column:

```c++
req.WithFilterExpression("ColumnValue > 10");
```

Finally, to select which columns you want to retrieve for each row, you use 
`SetProjectionExpression`:

```c++
req.SetProjectionExpression("Columns, you, want, to, retrieve");
```

If no `ProjectionExpression` is set, all columns will be returned.

### Running a Request

Next we'll need a loop to handle queries. This method takes a 
`QueryRequest` (req), runs it, and prints the returned values.

```c++
Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue> last_key;
std::cout << "driveid: " << drive_id.GetN() << std::endl;
do {
    auto error = client.Query(req);

    if (error.IsSuccess()) {
        auto result = error.GetResult();

        auto items = result.GetItems();

        for(auto it = items.begin(); it != items.end(); ++it) {
            std::cout << "\tlogtime: " << (*it).at("logtime").GetN() << std::endl;

            for(auto jt = (*it).begin(); jt != (*it).end(); ++jt) {
                if((*jt).first != "driveid" && (*jt).first != "logtime") {
                    Aws::String key = (*jt).first;
                    Aws::DynamoDB::Model::AttributeValue value = (*jt).second;

                    std::cout << "\t\t" << key << ": ";

                    if(key == "File_Name")
                        std::cout << value.GetS() << std::endl;
                    else
                        std::cout << value.GetN() << std::endl;
                }
            }
        }
        
        last_key = result.GetLastEvaluatedKey();
        
        req.WithExclusiveStartKey(last_key);
    }
    else {
        std::cout << "ERROR: " << error.GetError().GetMessage() << std::endl;
        break;
    }
}
while(last_key.size() != 0);
```

Note this loop:

```c++
        do {
            // ...
                last_key = result.GetLastEvaluatedKey();
                req.WithExclusiveStartKey(last_key);
            // ...
        } 
        while(last_key.size() != 0);
```

Queries and Scans to DynamoDB are limited to a certain size, measured in bytes. If you 
pull all the columns from the table, the size limit is hit at about 400 rows. 
To work around this, if a request hits the limit before all the results are 
returned, a `LastEvaluatedKey` attribute will have a value of the key of the 
row that it stopped on. You can take this key and assign it to the 
`ExclusiveStartKey` value of the request, run the request again, and 
it will start at that key.

Note that because a `GetItemRequest` returns only a single row it does not need the do-while loop and simply has to check whether the result was error-free or not.

```c++
auto result = client.GetItem(req);

if (result.IsSuccess()) {
    auto item = result.GetResult().GetItem();

    for(auto it = item.begin(); it != item.end(); ++it) {
        Aws::String key = (*it).first;
        Aws::DynamoDB::Model::AttributeValue value = (*it).second;

        std::cout << "\t" << key << ": ";

        if(key == "File_Name")
            std::cout << value.GetS() << std::endl;
        else
            std::cout << value.GetN() << std::endl;
    }
}
else
    std::cout << "ERROR: " << result.GetError().GetMessage() << std::endl;
```

### Accessing the results

Since the results are returned using a 
`Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue>>` for each row, 
they are very easy to access (Aws::Map and Aws::String are just specialized aliases of the C++ standard library implementations):

```c++
// To print the logtime of the first row
auto error = client.Query(req);
if(error.IsSuccess()) {
    std::cout << error.GetResult().GetItems()[0].at("logtime").GetN() std::endl;
}
```

The `AttributeValue` type, which is part of the DynamoDB library, has results 
by type. For this event, you will almost always call `.GetN()` to get the result's 
numerical type value. If you call the wrong type (like `.GetS()` on a numerical 
field) generally it will simply return an empty string.

Again, because `GetItemRequest` returns just a single row, it has the GetItem() method
as opposed to GetItems() and can bypass indexing into the vector. This means that it can access the
map slightly easier.
```c++
// To print the logtime of the item
auto error = client.Query(req);
if(error.IsSuccess()) {
    std::cout << error.GetResult().GetItem().at("logtime").GetN() std::endl;
}
```

## Working with S3

This covers basic AWS S3 interactions using C++.

Basic interactions are retrieving a file from the S3 bucket and writing/saving
that file locally.

The basic example of integration with S3 is outlined in 5-RetrieveFileFromS3.exe

**This section is a walkthrough of that code. To view the code directly, simply
go to the file in this folder.**

Here are the imported libraries required for S3:

```c++
#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h> 
#include <aws/s3/S3Client.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <fstream>
```

Declare a S3 client object and the name of the bucket you want to use:

```c++
Aws::Client::ClientConfiguration config;
Aws::Auth::AWSCredentials cred;
cred.SetAWSAccessKeyId("**********");
cred.SetAWSSecretKey("**********");

config.region = "us-east-1";

Aws::DynamoDB::DynamoDBClient client(cred, config);
```
> Note that in a production environment, the access key and secret key should 
not be hardcoded in your code files.
> You don't necessarily need to worry about this for this event.

The bucket to use is `p3na-18gus.3101.027`.

### Creating the S3 client to access the S3 API

Before any interactions, the S3 client object must be created so that the
S3 API can be accessed. In the example this is done at the start of the
`main` method:

```c++
Aws::Client::ClientConfiguration config;
config.region = "us-east-1";

Aws::S3::S3Client client(config);
```

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

```c++
Aws::S3::Model::GetObjectRequest req;

req.SetBucket("p3na-18gus.3101.027");
req.SetKey("this/is/a/key.mp4");
```

### Running a GetObjectRequest

Next we'll need a helper method to handle the request. This method receives a
`GetObjectRequest`, runs it, and returns the `GetObjectResponse` object.

```c++
auto request = client.GetObject(req);

if (request.IsSuccess()) {
    Aws::OFStream local_file;
    std::cout << "Writing out_file.mp4 to disk..." << std::endl;
    local_file.open("out_file.mp4", std::ios::out | std::ios::binary);
    local_file << request.GetResult().GetBody().rdbuf();
    local_file.close();
    std::cout << "Done!" << std::endl;
}
else
    std::cout << "ERROR: " << request.GetError().GetMessage() << std::endl;
```

### Saving the file locally

The response object, `GetObjectResponse` is rather oblique, but its main use is for writing/saving the file locally.

Once we have the `GetObjectResponse`, this is fairly easy.

```c++
local_file.open(save_location, std::ios::out | std::ios::binary);
local_file << request.GetResult().GetBody().rdbuf();
```
which opens a filestream in binary mode, and then writes the binary buffer of the retrieved file to that location.

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

## Sample queries

### Retrieve all data at a specific timestamp (`logtime`) (1-GetDataWithTimestamp.cpp)

This query demonstrates how to pull all rows for a drive scenario at a specific 
timestamp. In the event data, this should always return only one row.

```c++
Aws::DynamoDB::Model::GetItemRequest req;

Aws::DynamoDB::Model::AttributeValue timestamp;
Aws::DynamoDB::Model::AttributeValue drive_id;

req.SetTableName("honda-hackathon1");

timestamp.SetN("1365385");
req.AddKey("logtime", timestamp);
drive_id.SetN("20181218091707");
req.AddKey("driveid", drive_id);
```


### Retrieve all data from a specific drive scenario (2-QueryDriveScenario.cpp)

This query demonstrates how to pull all rows for a drive scenario.

```c++
Aws::DynamoDB::Model::QueryRequest req;

Aws::DynamoDB::Model::AttributeValue drive_id;

Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue> expression_attr;

req.SetTableName("honda-hackathon1");

drive_id.SetN("20181218091707");
expression_attr[":drive_id"] = drive_id;
req.SetExpressionAttributeValues(expression_attr);
req.SetKeyConditionExpression("driveid = :drive_id");
```

### Retrieve all data from the Table (3-ScanFullTable.cpp)

This scan pulls all the data in the table

```c++
Aws::DynamoDB::Model::ScanRequest req;

req.SetTableName("honda-hackathon1");
```

### Retrieve all GPS data for a drive scenario (4-QueryWithGPSFilter.cpp)

This query demonstrates how to pull all GPS data for a drive scenario. A 
`ProjectionExpression` is used to only retrieve the keys and the `GPS_Lat`, `GPS_Lon`, and 
`GPS_Alt` columns

```c++
Aws::DynamoDB::Model::QueryRequest req;

Aws::DynamoDB::Model::AttributeValue drive_id;
Aws::DynamoDB::Model::AttributeValue zero;

Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue> expression_attr;

req.SetTableName("honda-hackathon1");

drive_id.SetN("20181218091707");
expression_attr[":drive_id"] = drive_id;
drive_id.SetN("0");
expression_attr[":zero"] = zero;
req.SetExpressionAttributeValues(expression_attr);
req.SetKeyConditionExpression("driveid = :drive_id");
req.WithFilterExpression("GPS_Lat < :zero");

req.SetProjectionExpression("driveid, logtime, GPS_Lat, GPS_Lon, GPS_Alt");
```

### Save a file pulled from the S3 file server (5-RetrieveFileFromS3.cpp)

This file demonstrates how to download and save a file from S3
```c++
Aws::S3::Model::GetObjectRequest req;

req.SetBucket("p3na-18gus.3101.027");
req.SetKey("video-files/Recfile P3 Edge 20181127151525 67058417 Webcam Driver Outputiplimage.m4v");
```

## Further information
https://docs.aws.amazon.com/amazondynamodb/latest/developerguide/Introduction.html

https://docs.aws.amazon.com/sdk-for-cpp/v1/developer-guide/setup.html

### DynamoDB
http://sdk.amazonaws.com/cpp/api/LATEST/namespace_aws_1_1_dynamo_d_b.html

### S3
https://docs.aws.amazon.com/sdk-for-cpp/v1/developer-guide/examples-s3-objects.html

http://sdk.amazonaws.com/cpp/api/LATEST/namespace_aws_1_1_s3.html