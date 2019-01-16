var AWS = require("aws-sdk");
var fs = require("fs");
AWS.config.update({
    accessKeyId: "YOUR_ACCESS_KEY",
    secretAccessKey: "YOUR_SECRET_KEY"
  });

var S3 = new AWS.S3();

var BUCKET_NAME = "p3na-18gus.3101.027";
var objectPath = "video-files/";
var objectName = "Recfile P3 Edge 20181120 104743 Webcam Driver Outputiplimage.m4v";
var objectKey = objectPath + objectName;
var saveFilePath = "local/file/path/" + objectName;


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