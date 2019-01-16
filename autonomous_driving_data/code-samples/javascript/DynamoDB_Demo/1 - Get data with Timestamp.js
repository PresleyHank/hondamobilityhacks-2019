var AWS = require("aws-sdk");

AWS.config.update({
  region: "us-east-1",
  endpoint: "https://dynamodb.us-east-1.amazonaws.com",
  accessKeyId: "YOUR_ACCESS_KEY",
  secretAccessKey: "YOUR_SECRET_KEY"
});

var docClient = new AWS.DynamoDB.DocumentClient();

var driveScenarioId = 20181120104743;
var timestamp = 1376395;
var tablename = "honda-hackathon1";

var params = {
  TableName: tablename,
  ExpressionAttributeValues: { ":dId" : driveScenarioId, ":t": timestamp },
  KeyConditionExpression: "driveid = :dId and logtime = :t"
};

docClient.query(params, onScan);
var count = 0;

function onScan(err, data) {
  if(err) {
    console.error("Unable to query the table. Error JSON: ", JSON.stringify(err, null, 2));
  } else {
    console.log("Query succeeded");
    count += data.Items.length;

    // continue querying if we have more items
    if(typeof data.LastEvaluatedKey != "undefined") {
      console.log("Querying for more...");
      params.ExclusiveStartKey = data.LastEvaluatedKey;
      docClient.query(params, onScan);
    }
    console.log(`Found ${count} items`);
  }
}