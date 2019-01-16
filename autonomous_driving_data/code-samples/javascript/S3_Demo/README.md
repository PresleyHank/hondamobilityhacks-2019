# Javascript Code Example - S3
This covers basic AWS S3 interactions using Javascript. Basic interactions are retrieving a file from the S3 bucket, and writing/saving that file locally.

**The goal of this guide is to provide enough information for you to access the data without having to spend time on a deep-dive of the official documentation**

## Requirements
1. Your favourite Javascript IDE or Editor (Visual Studio Code)
2. Java AWS SDK
3. An AWS Access key, Secret Key, and region.

## AWS SDK Setup
You will need to add the following to your code in order to access the AWS SDK:
```js
var AWS = require("aws-sdk");
var S3 = new AWS.S3();
```

## Working with S3 and Creating Your Client
The Download File example is contained in `1 - Download File.js`. The API is powerful and gives you a massive number of options to do things with your bucket, but for our purposes this should get you started.

In order to connect to the S3 bucket, you will need to initialize the client variable with something like this: 
```js
var AWS = require("aws-sdk");
var S3 = new AWS.S3();
AWS.config.update({
    accessKeyId: "YOUR_ACCESS_KEY",
    secretAccessKey: "YOUR_SECRET_KEY"
  });
```

## Retrieving a File
Each file on S3 is stored in a `bucket`. The bucket is similar to a web address.

>For all purposes in getting drive scenario-related files, use the bucket p3na-18gus.3101.027

In S3, a file is known as an `object`. Each object has a key that is used to find it within the bucket.
The key is a combination of the path to the file within the bucket and the name of the file.

For example, if object `therockvsdwaynejohnson.mp4` was stored on the path `stop/hitting/yourself`, the key would be 
`stop/hitting/yourself/therockvsdwaynejohnson.mp4`.

The first step in retrieving a file is to create a `.getObject()` method calland set your parameters to include your bucket name and key. The below snippet of code creates the object, opens a `FileSync`to put the downloaded object into, then prints a completion notice to the console.
```js
S3.getObject( 
    { Bucket: BUCKET_NAME, Key: objectKey }, // params
    function(error, data) {
        if(error != null) {
            console.error("Failed to retrieve an object: " + error);
        } else {
            console("Loaded " + data.ContentLength + " bytes");
            // save data.Body to disk
            fs.writeFileSync(saveFilePath, data.Body.toString());
            console.log("${saveFilePath}` has been created!");
        }
    }
);
```

## Further Information
If you need more advanced functionality for your application, you are going to want this:
https://docs.aws.amazon.com/AmazonS3/latest/dev/RetrievingObjectUsingNetSDK.html