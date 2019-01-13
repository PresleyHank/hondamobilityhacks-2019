using System;
using System.Text;
using System.Threading.Tasks;
using System.Collections.Generic;
using Amazon.DynamoDBv2.Model;

namespace HondaHackathonExampleCode
{
    partial class Program
    {
        /// <summary>
        /// Queries for all data of a drive scenario.
        /// </summary>
        /// <param name="driveID">The drive scenario ID.</param>
        /// <returns>Log data for the entire drive scenario.</returns>
        public static async Task<List<Dictionary<string, AttributeValue>>> QueryDriveScenario(string driveID)
        {
            var qRequest = new QueryRequest
            {
                TableName = tableName,
                ExpressionAttributeValues = new Dictionary<string, AttributeValue>
                {
                    // set a query value for the drive ID value as a number.
                    { ":dId", new AttributeValue { N = driveID } }
                },
                // get all results for the drive ID
                KeyConditionExpression = "driveid = :dId"
            };

            var queryTask = await ClientQuerying_async(qRequest);

            return queryTask;
        }

        /// <summary>
        /// Queries for all data at a specific timestamp (1 log).
        /// </summary>
        /// <param name="driveID">The drive scenario ID.</param>
        /// <param name="time">The timestamp of the log.</param>
        /// <returns>Log data at the specific timestamp.</returns>
        public static async Task<List<Dictionary<string, AttributeValue>>> QuerySpecificTimestamp(string driveID, string time)
        {
            var qRequest = new QueryRequest
            {
                TableName = tableName,
                ExpressionAttributeValues = new Dictionary<string, AttributeValue>
                {
                    // set a query value for the drive ID value as a number.
                    { ":dId", new AttributeValue { N = driveID } },
                    // set a query value for the logtime/timestamp value as a number.
                    { ":t", new AttributeValue { N = time } }
                },
                // get all results for the drive ID where the logtime is equal to the timestamp
                KeyConditionExpression = "driveid = :dId and logtime = :t"
            };

            var queryTask = await ClientQuerying_async(qRequest);

            return queryTask;
        }

        /// <summary>
        /// Queries for all GPS data of a drive scenario.
        /// </summary>
        /// <param name="driveID">The drive scenario ID.</param>
        /// <returns>GPS data.</returns>
        public static async Task<List<Dictionary<string, AttributeValue>>> QueryAllGpsData(string driveID)
        {
            var qRequest = new QueryRequest
            {
                TableName = tableName,
                ExpressionAttributeValues = new Dictionary<string, AttributeValue>
                {
                    // set a query value for the drive ID value as a number.
                    { ":dId", new AttributeValue { N = driveID } }
                },
                // get all results for the drive ID
                KeyConditionExpression = "driveid = :dId",
                // only get the GPS_Lat, GPS_Lon, and GPS_Alt columns
                ProjectionExpression = "GPS_Lat, GPS_Lon, GPS_Alt"
            };

            var queryTask = await ClientQuerying_async(qRequest);

            return queryTask;
        }

        /// <summary>
        /// Queries the database with the given query request.
        /// 
        /// If the query hits a certain size limit, the query will end with a LastEvaluatedKey value.
        /// We use a loop to run the query over and over until there is no LastEvaluatedKey.
        /// Each new query uses the previous query's LastEvaluatedKey as its ExclusiveStartKey.
        /// This tells each iteration where to begin.
        /// </summary>
        /// <param name="qRequest">A query request.</param>
        /// <returns>A list of rows (rows are returned as dictionaries with keys as each requested attribute).</returns>
        private static async Task<List<Dictionary<string, AttributeValue>>> ClientQuerying_async(QueryRequest qRequest)
        {
            // since queries have a size limit, we need to append the results of each one to a 'master' list
            var results = new List<Dictionary<string, AttributeValue>> { };

            QueryResponse qResponse;

            do
            {
                try
                {
                    // run the task
                    qResponse = await client.QueryAsync(qRequest);
                    // add the results of the size-limited query to the overall results
                    results.AddRange(qResponse.Items);
                    // add the last evaluated key to the next query so it knows where to start
                    qRequest.ExclusiveStartKey = qResponse.LastEvaluatedKey;
                }
                catch (Exception ex)
                {
                    Console.WriteLine("      The low-level query FAILED, because:\n       {0}.", ex.Message);
                    return new List<Dictionary<string, AttributeValue>>();
                }
            } while (qResponse.LastEvaluatedKey != null && qResponse.LastEvaluatedKey.Count != 0);

            Console.WriteLine("     -- Retrieved {0} logs.", results.Count);

            return results;
        }

        /// <summary>
        /// Prints the timestamp from a log.
        /// 
        /// This will fail if the projection of the row does not include the logtime attribute.
        /// </summary>
        /// <param name="log">A log (row) from a database query.</param>
        private static void PrintLogTime(Dictionary<string, AttributeValue> log)
        {
            string tsAttr = "logtime";

            if (log.ContainsKey(tsAttr))
            {
                Console.WriteLine(log[tsAttr].N);
            }
            else
            {
                Console.WriteLine("Log does not contain the \"{0}\" attribute.", tsAttr);
            }
        }

        /// <summary>
        /// Prints the GPS data from a log.
        /// 
        /// This will fail if the projection of the row does not include the required GPS attributes.
        /// </summary>
        /// <param name="log">A log (row) from a database query.</param>
        private static void PrintGpsData(Dictionary<string, AttributeValue> log)
        {
            string latAttr = "GPS_Lat";
            string lonAttr = "GPS_Lon";
            string altAttr = "GPS_Alt";

            var gpsData = new StringBuilder();

            // check for latitude
            if (log.ContainsKey(latAttr))
            {
                gpsData.Append("Lat: " + log[latAttr].N + ", ");
            }
            else
            {
                Console.WriteLine("Log does not contain the \"{0}\" attribute.", latAttr);
            }

            // check for longitude
            if (log.ContainsKey(lonAttr))
            {
                gpsData.Append("Lon: " + log[lonAttr].N + ", ");
            }
            else
            {
                Console.WriteLine("Log does not contain the \"{0}\" attribute.", lonAttr);
            }

            // check for altitude
            if (log.ContainsKey(altAttr))
            {
                gpsData.Append("Alt: " + log[altAttr].N);
            }
            else
            {
                Console.WriteLine("Log does not contain the \"{0}\" attribute.", altAttr);
            }

            Console.WriteLine(gpsData.ToString());
        }
    }
}
