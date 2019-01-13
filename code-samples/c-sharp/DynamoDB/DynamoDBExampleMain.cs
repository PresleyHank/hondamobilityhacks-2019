using System;
using Amazon.DynamoDBv2;

namespace HondaHackathonExampleCode
{
    partial class Program
    {
        // AWS DynamoDB API access variables
        private static AmazonDynamoDBClient client;

        private static readonly string tableName = "honda-hackathon1";

        static void Main(string[] args)
        {
            CreateClient();

            string driveScenarioID = "desired drive id";

            var driveScenarioQueryResults = QueryDriveScenario(driveScenarioID).GetAwaiter().GetResult();
            var timestampQueryResults = QuerySpecificTimestamp(driveScenarioID, "desired time stamp").GetAwaiter().GetResult();
            var gpsDataOnlyResults = QueryAllGpsData(driveScenarioID).GetAwaiter().GetResult();

            PrintLogTime(driveScenarioQueryResults[0]);
            PrintLogTime(timestampQueryResults[0]);
            PrintGpsData(gpsDataOnlyResults[0]);
        }

        /// <summary>
        /// Creates the AWS DynamoDB client, which is used to access the AWS API.
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
                client = new AmazonDynamoDBClient(accessKey, secretKey, region);
            }
            catch (Exception ex)
            {
                Console.WriteLine("     FAILED to create a DynamoDB client; " + ex.Message);
            }
        }
    }
}
