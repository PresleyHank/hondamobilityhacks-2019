# Python S3 Code Example

Downloading files from S3 buckets is very simple using Python and Boto3.

## Requirements
1. Python 3 (Recommended version 3.7.2+) 
2. Python Pip
3. Boto 3 Libary for working with AWS. [https://boto3.amazonaws.com/v1/documentation/api/latest/index.html]
4. An AWS Access Key, Secret Key, and Region.  These will be distributed from the staff at the event.

## Working with S3

All examples are located in a Python script called 'Sample.py'.

**This section is a walkthrough of that code. To view the code directly, simply
go to the two files in this folder.**

Here are the imported libraries required for S3:

```python
import boto3
import botocore
```

Declare a S3 client object and the name of the bucket you want to use:

```python
s3 = boto3.resource('s3',
                    aws_access_key_id='************************',
                    aws_secret_access_key='***********************************',
                    region_name='us-east-1'
                    )
```

The bucket to use is `p3na-18gus.3101.027`.

> Note that in a production environment, the access key and secret key should
> not be hardcoded in your code files.
> You don't necessarily need to worry about this for this event. If you're
> interested though, here's more information on it:
> https://boto3.amazonaws.com/v1/documentation/api/latest/guide/quickstart.html#configuration

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

With Python, retrieving a file is as simple as calling `.download_file()` from the bucket.

Here's the structure of creating the request:

```python
    s3.Bucket(BUCKET_NAME).download_file(KEY, local_file_name)
```

### Saving the file locally

In Boto3, `download_file()` will automatically save the file if we pass the second parameter. Above this is listed as `local_file_name`.

## Sample retrieval and save

In the example code, we have created some preloaded variables at the top of the script to allow for easy customization
and reading.

```python
    import boto3
    import botocore
    
    # Define the S3 Bucket Name
    BUCKET_NAME = 'p3na-18gus.3101.027'
    # Path within the S3 bucket to folder we desire
    PATH = 'video-files/'
    SAVE_PATH = 'downloaded-files/'
    # File name we wish to download
    file_name = 'Recfile P3 Edge 20181120 104743 Webcam Driver Outputiplimage.m4v'
    KEY = PATH + file_name
    
    # Establish the AWS client connection using access keys.
    # Select the correct AWS resource
    s3 = boto3.resource('s3',
                        aws_access_key_id='**************************',
                        aws_secret_access_key='*************************',
                        region_name='us-east-1'
                        )
    
    # Download file from S3 bucket, and store at local location 'file_name'.
    try:
        s3.Bucket(BUCKET_NAME).download_file(KEY, SAVE_PATH + file_name)
    except botocore.exceptions.ClientError as e:
        if e.response['Error']['Code'] == "404":
            print("The object does not exist.")
        else:
            raise

```

After execution of this code, we will see that we have a our file saved locally in the 'downloaded-files' directory 
 under our sample folder.  This folder must be generated prior to script execution or else it will fail. 

## Further information

https://docs.aws.amazon.com/AmazonS3/latest/dev/RetrievingObjectUsingNetSDK.html
