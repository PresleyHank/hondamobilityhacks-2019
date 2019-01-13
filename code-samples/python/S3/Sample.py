import boto3
import botocore

# Define the S3 Bucket Name
BUCKET_NAME = 'p3na-18gus.3101.027'
# Path within the S3 bucket to folder we desire
PATH = 'video-files/'
# File name we wish to download
file_name = 'Recfile P3 Edge 20181120 104743 Webcam Driver Outputiplimage.m4v'
KEY = PATH + file_name

# Establish the AWS client connection using access keys.
# Select the correct AWS resource
s3 = boto3.resource('s3',
                    aws_access_key_id='**********************',
                    aws_secret_access_key='***********************',
                    region_name='us-east-1'
                    )

# Download file from S3 bucket, and store at local location 'file_name'.
try:
    s3.Bucket(BUCKET_NAME).download_file(KEY, file_name)
except botocore.exceptions.ClientError as e:
    if e.response['Error']['Code'] == "404":
        print("The object does not exist.")
    else:
        raise
