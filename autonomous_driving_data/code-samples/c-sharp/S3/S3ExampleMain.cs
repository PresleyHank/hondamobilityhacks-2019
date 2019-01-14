using System;
using Amazon.S3;

namespace AwsBucketExample
{
    partial class Program
    {
        // AWS S3 access variables
        private static AmazonS3Client client;

        private static readonly string bucketName = "p3na-18gus.3101.027";

        static void Main(string[] args)
        {
            CreateClient();

            string objectPath = "video-files/";
            string objectName = "Recfile P3 Edge 20181120 104743 Webcam Driver Outputiplimage.m4v";
            // key for the object location within the bucket
            string objectKey = objectPath + objectName;
            // local path to save the downloaded file to.
            string saveFilePath = "local/file/path/" + objectName;

            var downloadFileResult = DownloadFile(objectKey, saveFilePath).GetAwaiter().GetResult();
        }

        /// <summary>
        /// Creates the AWS S3 client, which is used to access the AWS API.
        /// 
        /// Requires an access key and secret key, as well as defining the AWS region.
        /// Those credentials will be provided by hackathon organizers.
        /// </summary>
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
    }
}
