package java_demo;

import com.amazonaws.AmazonClientException;
import com.amazonaws.AmazonServiceException;
import com.amazonaws.auth.AWSStaticCredentialsProvider;
import com.amazonaws.auth.BasicAWSCredentials;
import com.amazonaws.auth.profile.ProfileCredentialsProvider;
import com.amazonaws.regions.Regions;
import com.amazonaws.services.dynamodbv2.AmazonDynamoDB;
import com.amazonaws.services.dynamodbv2.AmazonDynamoDBClientBuilder;

import com.amazonaws.services.dynamodbv2.model.*;
import sun.font.AttributeValues;

import javax.management.AttributeList;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


public class SampleQueries {
    private static AmazonDynamoDB client;
    private static String TABLE_NAME = "honda-hackathon1";
    private static String ACCESS_KEY = "YOUR_ACCESS_KEY_HERE";
    private static String SECRET_KEY = "YOUR_SECRET_KEY_HERE";

    /**
     * Method initDb() creates the AWS DynamoDB client, which is used to access the AWS API
     *
     * Requires an access key and secret key, as well as defining the AWS region.
     * Those credentials will be provided by hackathon organizers
     */
    private static void initDb() {
        BasicAWSCredentials awsCredentials = new BasicAWSCredentials(ACCESS_KEY, SECRET_KEY);

        client = AmazonDynamoDBClientBuilder.standard()
                .withCredentials(new AWSStaticCredentialsProvider(awsCredentials))
                .withRegion(Regions.US_EAST_1)
                .build();
    }

    public static void main(String[] args) throws Exception {
        String driveScenarioId = "20181120104743";
        String timestamp = "1376395";

        initDb();
        try {
            System.out.println("Running queries...");
            System.out.println("This may take a few moments...");
            System.out.println("===============================");

            queryDriveScenario(driveScenarioId);
            querySpecificTimestamp(driveScenarioId, timestamp);
            queryAllGpsData(driveScenarioId);
        } catch (AmazonServiceException ase) {
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

    /**
     * Queries for all data of a drive scenario and prints the record count to the console
     * @param driveScenarioId The drive scenario ID
     */
    private static void queryDriveScenario(final String driveScenarioId) {
        List<Map<String, AttributeValue>> results = new ArrayList<Map<String, AttributeValue>>();
        QueryRequest queryRequest = new QueryRequest()
                .withTableName(TABLE_NAME)
                .withExpressionAttributeValues(
                        new HashMap<String, AttributeValue>() {{
                            // set a query value for the drive id value as a number
                            put(":dId", new AttributeValue().withN(driveScenarioId));
                        }})
                // get all results for the drive ID
                .withKeyConditionExpression("driveid = :dId");

        QueryResult queryResult;
        do {
            queryResult = client.query(queryRequest);
            results.addAll(queryResult.getItems());
            queryRequest.setExclusiveStartKey(queryResult.getLastEvaluatedKey());
        } while(queryResult.getLastEvaluatedKey() != null && queryResult.getLastEvaluatedKey().size() != 0);
        System.out.println("Retrieved " + results.size() + " logs for Drive Scenario ID " + driveScenarioId);

    }

    /**
     * Queries for all data at a specific timestamp (1 log).
     * @param driveScenarioId the drive scnario ID
     * @param timestamp the timestamp of the log
     */
    private static void querySpecificTimestamp(final String driveScenarioId, final String timestamp) {
        List<Map<String, AttributeValue>> results = new ArrayList<Map<String, AttributeValue>>();
        QueryRequest queryRequest = new QueryRequest()
                .withTableName(TABLE_NAME)
                .withExpressionAttributeValues(
                        new HashMap<String, AttributeValue>() {{
                            put(":dId", new AttributeValue().withN(driveScenarioId));
                            put(":t", new AttributeValue().withN(timestamp));
                        }}
                )
                .withKeyConditionExpression("driveid = :dId and logtime = :t");

        QueryResult queryResult;
        do {
            queryResult = client.query(queryRequest);
            results.addAll(queryResult.getItems());
            queryRequest.setExclusiveStartKey(queryResult.getLastEvaluatedKey());
        } while(queryResult.getLastEvaluatedKey() != null && queryResult.getLastEvaluatedKey().size() != 0);
        System.out.println("Retrieved " + results.size() + " logs at timestamp " + timestamp
                + " with Drive Scenario ID " + driveScenarioId);

    }

    /**
     * Queries for all GPS data of a drive scenario
     * @param driveScenarioId the drive scenario ID
     */
    private static void queryAllGpsData(final String driveScenarioId) {
        List<Map<String, AttributeValue>> results = new ArrayList<Map<String, AttributeValue>>();
        QueryRequest queryRequest = new QueryRequest()
                .withTableName(TABLE_NAME)
                .withExpressionAttributeValues(
                        new HashMap<String, AttributeValue>() {{
                            put(":dId", new AttributeValue().withN(driveScenarioId));
                        }}
                )
                .withProjectionExpression("GPS_Lat, GPS_Lon, GPS_Alt")
                .withKeyConditionExpression("driveid = :dId");

        QueryResult queryResult;
        do {
            queryResult = client.query(queryRequest);
            results.addAll(queryResult.getItems());
            queryRequest.setExclusiveStartKey(queryResult.getLastEvaluatedKey());
        } while(queryResult.getLastEvaluatedKey() != null && queryResult.getLastEvaluatedKey().size() != 0);
        System.out.println("Retrieved " + results.size() + " GPS logs for Drive Scenario ID " + driveScenarioId);
    }
}
