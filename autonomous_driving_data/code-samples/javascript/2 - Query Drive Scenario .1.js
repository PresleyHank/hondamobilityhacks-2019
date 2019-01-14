var AWS = require("aws-sdk");

AWS.config.update({
  region: "us-west-2",
  endpoint: "https://dynamodb.us-west-2.amazonaws.com",
  accessKeyId: "AKIAI5HCDUERQ6VUWIFQ",
  secretAccessKey: "VN/eJcJ8Dj4UHXI4Lqs+c+JIn+RJ+6+SKUAhIT9F"
});

var docClient = new AWS.DynamoDB.DocumentClient();

var timestamp = "1039012";

var params = {
  TableName: "honda-sample2",
  Key: {
    Timestamp: timestamp
  }
};

docClient.query(params, (err, data) => {
  if (err) {
    console.error("Error processing query:", JSON.stringify(err, null, 2));
  } else {
    console.log("Received Data:", JSON.stringify(data, null, 2));
  }
});
