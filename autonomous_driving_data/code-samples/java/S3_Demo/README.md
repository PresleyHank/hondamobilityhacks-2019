# Java Code Example - S3

This covers basic AWS S3 interactions using Java. Basic interactions are retriving a file from the S3 bucket and 
writing/saving that file locally.

**The goal of this guide is to provide enough information fo ryou to access the data without having to spend time on a 
deep-dive of the official documentation.**

## Requirements

1. Your favourite Java IDE (IntelliJ IDEA)
2. Java AWS SDK v.1.11.478
3. An AWS Access key, Secret key, and Region

## AWS SDK Setup

You will need to add the following to your `pom.xml` file for your project:
```xml
<dependencyManagement>
    <dependencies>
        <dependency>
            <groupId>com.amazonaws</groupId>
            <artifactId>aws-java-sdk-bom</artifactId>
            <version>1.11.478</version>
            <type>pom</type>
            <scope>import</scope>
        </dependency>
    </dependencies>
</dependencyManagement>

<dependencies>
    <dependency>
        <groupId>com.amazonaws</groupId>
        <artifactId>aws-java-sdk-ec2</artifactId>
    </dependency>
    <dependency>
        <groupId>com.amazonaws</groupId>
        <artifactId>aws-java-sdk-s3</artifactId>
    </dependency>
    <dependency>
        <groupId>com.amazonaws</groupId>
        <artifactId>aws-java-sdk-dynamodb</artifactId>
    </dependency>
</dependencies>

```

This will automatically import the SDK for you instead of having to deal with downloading .jars and managing them. You can
also use Gradle if you prefer, but Maven is the fastest route since it is what Amazon directly supports and uses.
You have the option of using the new AWS 2.0 version, however, in the writing of this demonstration, 99% of the code samples
available both on Amazon's documentation site as well as third party tutorial sites used 1.11 for their examples. 2.0 is 
a **MASSIVE** change to the API's structure, so consider this your warning. If you are already familiar with 2.0, then 
you can ignore most of this document.

## Working with S3 and Creating Your Client

All examples are contained in a single file called SampleInteractions.java. The API is powerful and gives you a massive 
number of options to do things with your bucket, but for our purposes this should get you started. 

In order to connect to the S3 bucket, you will need a client initialization method that can be called from your main method:

```java
private static void createClient() {
        BasicAWSCredentials awsCredentials = new BasicAWSCredentials(ACCESS_KEY, SECRET_KEY);
        client = AmazonS3ClientBuilder.standard()
                .withCredentials(new AWSStaticCredentialsProvider(awsCredentials))
                .withRegion(Regions.US_EAST_1)
                .build();
    }

    public static void main(String[] args) {
        // initialize our client
        createClient();
        ...
        ...
        ...
}
```

You will also want to catch two potential exceptions from the main method:
```java
 public static void main(String[] args) {
        // initialize our client
        createClient();

       

        try {
            // S3 method calls go here
            ...
            ...
            ...
        }catch (AmazonServiceException ase) {
            System.out.println("Caught an AmazonServiceException, which means your request made it "
                    + "to AWS, but was rejected with an error response for some reason.");
            System.out.println("Error Message:    " + ase.getMessage());
            System.out.println("HTTP Status Code: " + ase.getStatusCode());
            System.out.println("AWS Error Code:   " + ase.getErrorCode());
            System.out.println("Error Type:       " + ase.getErrorType());
            System.out.println("Request ID:       " + ase.getRequestId());
        } catch (AmazonClientException ace) {
            System.out.println("Caught an AmazonClientException, which means the client encountered "
                    + "a serious internal problem while trying to communicate with AWS, "
                    + "such as not being able to access the network.");
            System.out.println("Error Message: " + ace.getMessage());
        }

    }
```
This will help you debug your application if you manage to get unhandled exceptions.

## Retrieving a File

Each file on S3 is stored in a `bucket`. The bucket is similar to a web address.

>For all purposes in getting drive scenario-related files, use the bucket p3na-18gus.3101.027

In S3, a file is known as an `object`. Each object has a key that is used to find it within the bucket.
The key is a combination of the path to the file within the bucket and the name of the file.

For example, if object `therockvsdwaynejohnson.mp4` was stored on the path `stop/hitting/yourself`, the key would be 
`stop/hitting/yourself/therockvsdwaynejohnson.mp4`.

The first step in retrieving a file is to create a `.getObject` with the bucket and the key. The below snippet of code
creates the object, opens an `S3ObjectInputStream` and a `FileOutputStream` to put the downloaded object into, then closes 
the streams on completion.
```java
S3Object object = client.getObject(BUCKET_NAME, objectKey);
S3ObjectInputStream inputStream = object.getObjectContent();
try {
    // create file stream to a local file
    FileOutputStream fileOutputStream = new FileOutputStream(new File(saveFilePath));
    byte[] read_buf = new byte[1024];
    int read_len;
    // write the stream
    while((read_len = inputStream.read(read_buf)) > 0) {
        fileOutputStream.write(read_buf, 0, read_len);
    }
    // close the streams
    inputStream.close();
    fileOutputStream.close();
} catch(IOException e) {
    System.out.println(e.getMessage());
}
```

## Sample Retrieval and Save Method

```java
/**
     * Downloads the file at the specified key in a bucket and saves it at the specified location
     * @param objectKey The key of the object, which includes the path from the bucket and the file name
     * @param saveFilePath where you want to save the file to on your computer
     */
private static void downloadFile(String objectKey, String saveFilePath) {
    // create S3Object with the bucket name and object's key
    S3Object object = client.getObject(BUCKET_NAME, objectKey);
    S3ObjectInputStream inputStream = object.getObjectContent();
    try {
        // create file stream to a local file
        FileOutputStream fileOutputStream = new FileOutputStream(new File(saveFilePath));
        byte[] read_buf = new byte[1024];
        int read_len;
        // write the stream
        while((read_len = inputStream.read(read_buf)) > 0) {
            fileOutputStream.write(read_buf, 0, read_len);
        }
        // close the streams
        inputStream.close();
        fileOutputStream.close();
    } catch(IOException e) {
        System.out.println(e.getMessage());
    }
}
```

## Further Information
If you need more advanced functionality for your application, you are going to want these. MAKE SURE THAT YOU USE
DOCUMENTATION THAT IS BOTH UP TO DATE AND FOR API v1.11.478

https://docs.aws.amazon.com/AmazonS3/latest/dev/RetrievingObjectUsingNetSDK.html