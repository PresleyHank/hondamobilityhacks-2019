# January 2018 Honda Hackathon README <br>
## Generic AWS Information: <br>
**Region**: us-east-1<br>

## Dynamodb Information:
**Tablename**: 'honda-hackathon1'<br>
API Credentials will be distributed to each team by on site hackathon support staff.<br>

## S3 Storage Information <br>
**S3 Bucket Name**: 'p3na-18gus.3101.027'<br>
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

### General Information: <br>
**File Structure**:<br>
/data-files  (Contains a CSV version of the DynamoDB Database.)<br>
/rec-files  (Contains dSpace .rec files for all sample drives.)<br>
/sample-files  (Contains additional sample files.)<br>
/video-files  (Contains video files for each drive, as referenced in Dynamodb.)<br>

### Web Based Access: <br>
A web browser can be utilized to access and navigate the S3 storage bucket.<br>
**URL**: http://p3na-18gus.3101.027.s3-website-us-east-1.amazonaws.com/ <br>

## Sample Code Information: <br>
Within this folder you will find sample code examples for five popular programming languages, as well as the 
information you will need for accessing the database and additional resources for your hackathon project.<br>
**Use Cases**:<br>
1. Pull records for a specific time stamp.
2. Pull records for a specific drive scenario.
3. Pull the entire table. 
4. Show how to use filters. (Get GPS only)
5. Authenticate with provided username/key.
6. Getting a specific drive set of videos.

**Data Possibilities** <br>
- Pull by scenario id
- Pull by scenario type
- Pull by weather type