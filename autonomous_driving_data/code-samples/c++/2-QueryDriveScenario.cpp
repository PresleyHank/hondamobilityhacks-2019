// Queries all data for a particular driveid

#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h> 
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/AttributeDefinition.h>
#include <aws/dynamodb/model/QueryRequest.h>
#include <aws/core/auth/AWSCredentialsProvider.h>

#include <iostream>

int main(int argc, char* argv[]) {
    if(argc < 3) {
        std::cerr << "Please provide an AWS Access Key ID and AWS Secret Key" << std::endl;
        return 0;
    }

    // Start our AWS block
    Aws::SDKOptions options;
    Aws::InitAPI(options);

    Aws::Client::ClientConfiguration config;
    Aws::Auth::AWSCredentials cred;
    cred.SetAWSAccessKeyId(argv[1]);
    cred.SetAWSSecretKey(argv[2]);

    // Set our region
    config.region = "us-east-1";

    // The client we'll use to interact with the database
    Aws::DynamoDB::DynamoDBClient client(cred, config);

    // The request we'll be making to the client
    Aws::DynamoDB::Model::QueryRequest req;

    // The attribute we'll be retrieving based on
    Aws::DynamoDB::Model::AttributeValue drive_id;

    // And a map so we can bind our attributes to the request
    Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue> expression_attr;

    // Declare the table we want to search on
    req.SetTableName("honda-hackathon1");

    // Assign a value to our primary key and bind it to the query
    drive_id.SetN("20181120104743");
    expression_attr[":drive_id"] = drive_id;
    req.SetExpressionAttributeValues(expression_attr);
    req.SetKeyConditionExpression("driveid = :drive_id");

    // This map will hold the last key that we saw on the previous query
    // this allows us to continue querying the database until all results have
    // been returned
    Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue> last_key;

    bool first = true;
    Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue> first_item;
    int num_logs = 0;

    std::cout << "driveid: " << drive_id.GetN() << std::endl;
    do {
        // Get results
        auto error = client.Query(req);

        // If successful
        if (error.IsSuccess()) {
            auto result = error.GetResult();

            // Separate out the data part of the returned value
            auto items = result.GetItems();

            num_logs += items.size();

            // Iterate over every returned item
            for(auto it = items.begin(); it != items.end(); ++it) {
                std::cout << "\tlogtime: " << (*it).at("logtime").GetN() << "\n";

                if(first) {
                    first = false;
                    first_item = *it;
                }

                // And within every item iterate over every attribute
                for(auto jt = (*it).begin(); jt != (*it).end(); ++jt) {
                    // We dealt with driveid in our selection, and
                    // logtime is taken care of outside of this loop 
                    // (as it's a key for this database), so we're ignoring them in
                    // this loop
                    if((*jt).first != "driveid" && (*jt).first != "logtime") {
                        Aws::String key = (*jt).first;
                        Aws::DynamoDB::Model::AttributeValue value = (*jt).second;

                        std::cout << "\t\t" << key << ": ";

                        // It's important to know what type of attribute
                        // each key holds
                        if(key == "File_Name" || key == "Weather_Conditions" || 
                            key == "Driver_Video" || key == "Exterior_Video") {
                            std::cout << value.GetS() << "\n";
                        }
                        else
                            std::cout << value.GetN() << "\n";
                    }
                }
            }
            
            // Save the last evaluated key so we can use it in our next query
            last_key = result.GetLastEvaluatedKey();
            
            // And push that key into our request for the next query we make 
            req.WithExclusiveStartKey(last_key);
        }
        // Else tell us what went wrong
        else {
            std::cout << "ERROR: " << error.GetError().GetMessage() << std::endl;
            break;
        }
    }
    // Continue querying the database until we have retrieved everything that
    // matches our query
    while(last_key.size() != 0);
                    
    std::cerr << "\t-- Retrieved " << num_logs << " logs." << std::endl;
    std::cerr << first_item.at("logtime").GetN() << std::endl;

    // And clean up after ourselves
    Aws::ShutdownAPI(options);

    return 0;
}