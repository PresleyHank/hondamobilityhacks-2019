using System;
using System.Threading;
using System.Threading.Tasks;
using Amazon.S3.Model;

namespace AwsBucketExample
{
    partial class Program
    {
        /// <summary>
        /// Downloads the file at the specified key in a bucket and saves it at the specified location.
        /// </summary>
        /// <param name="objectKey">The key of the object, which includes the path from the bucket and the file name.</param>
        /// <param name="saveFilePath">Where you want to save the file to on your computer.</param>
        /// <returns>The Get Object response.</returns>
        private static async Task<GetObjectResponse> DownloadFile(string objectKey, string saveFilePath)
        {
            // build the get object request
            var getObjectRequest = new GetObjectRequest()
            {
                BucketName = bucketName,
                Key = objectKey
            };
            // run GetObject and store the response
            var getObjectResponse = await GetObject_async(getObjectRequest);
            // run SaveObject, which saves the object on this computer
            await SaveObject_async(getObjectResponse, saveFilePath);

            return getObjectResponse;
        }

        /// <summary>
        /// Retrieves an object from AWS S3 with instructions from the given GetObjectRequest.
        /// </summary>
        /// <param name="getObjectRequest">Instructions on which object to retrieve from the S3 bucket.</param>
        /// <returns>The GetObjectResponse with the retrieved object.</returns>
        private static async Task<GetObjectResponse> GetObject_async(GetObjectRequest getObjectRequest)
        {
            GetObjectResponse getObjectResponse;

            try
            {
                // run the task
                getObjectResponse = await client.GetObjectAsync(getObjectRequest);
            }
            catch (Exception ex)
            {
                Console.WriteLine("      The get object FAILED, because:\n       {0}.", ex.Message);
                return new GetObjectResponse();
            }

            Console.WriteLine("     -- Retrieved object with key {0} from bucket {1}.",
                getObjectResponse.Key, getObjectResponse.BucketName);

            return getObjectResponse;
        }

        /// <summary>
        /// Saves an object locally that was retrieved from S3 at the desired location.
        /// </summary>
        /// <param name="getObjectResponse">The response from S3 containing the retrieved object.</param>
        /// <param name="saveFilePath">Where to save the file locally.</param>
        /// <returns></returns>
        private static async Task<bool> SaveObject_async(GetObjectResponse getObjectResponse, string saveFilePath)
        {
            var source = new CancellationTokenSource();
            var token = source.Token;

            try
            {
                await getObjectResponse.WriteResponseStreamToFileAsync(saveFilePath, false, token);
            }
            catch (Exception ex)
            {
                Console.WriteLine("      The save object FAILED, because:\n       {0}.", ex.Message);
                return false;
            }

            return true;
        }
    }
}
