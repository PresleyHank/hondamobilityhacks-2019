# Autonomous Driving Data

In this repo you'll find event information, data, and code samples for various 
programming languages and how to access the data and associated files.

1. [General Information](#general-information)
2. [Data and File Access](#data-and-file-access)

## General Information

The data contained within this category was collected and provided by Continental and P3.  For questions or issues with
accessing the date, please seek out the support staff from P3.

## Data and File Access

### Access

Event data and files are hosted on AWS DynamoDB and S3.

Read-only AWS access is granted through an Access ID and an Access Secret Key. 
These will be provided to you. You also must provide an AWS region. 
Always use the region that represents `US East 1` (the exact name varies with 
language).

To access data on DynamoDB, reference the sample code for the language of 
choice. Each language has both sample code and documentation explaining the 
basic setup of DynamoDB and different ways to query.

To access files on S3, either go to [this link for a direct download](http://p3na-18gus.3101.027.s3-website-us-east-1.amazonaws.com/) 
or reference the sample code for the language of choice to programmatically 
download files. Each language also has sample code and documentation explaining 
S3.

S3 bucket name: `p3na-18gus.3101.027`

### Descriptions and Help

As stated, this repo contains sample code and documentation for various 
languages. The languages are C++, C#, Java, JavaScript, and Python. Reference 
the READMEs (there are multiple) for each language.

Descriptions/definitions of the data can be found in [the DataDefinitions.md file](./DataDefinitions.md).
