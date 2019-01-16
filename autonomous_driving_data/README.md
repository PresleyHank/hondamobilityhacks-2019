# Autonomous Driving Data

In this directory you will find information about the Autonomous Driving dataset. You will also find `code samples` for various programming languages showing you how to access the data.

## Table of contents

1. [Data](#data)
2. [Sample Code](#sample-code)
3. [Access](#access)

## Data

Event data and files are hosted on AWS DynamoDB and S3.

Descriptions and definitions of the data can be found in the [DataDefinitions.md](./DataDefinitions.md) file.

## Sample Code

The sample codes are provided in C++, C#, Java, JavaScript, and Python. Reference the READMEs (there are multiple) for each language.

## Access

Read-only AWS access is granted through an `Access ID` and an `Access Secret Key`. 
These will be provided to you. You also must provide an AWS region. 
Always use the region that represents `US East 1` (the exact name varies with language).

To access data on DynamoDB, reference the sample code for the language of choice. Each language has both sample code and documentation explaining the basic setup of DynamoDB and different ways to query.

To access files on S3, either go to this [link](http://p3na-18gus.3101.027.s3-website-us-east-1.amazonaws.com/) for a direct download or reference the sample code for the language of choice to programmatically download files. Each language also has sample code and documentation explaining S3.

S3 bucket name: `p3na-18gus.3101.027`


