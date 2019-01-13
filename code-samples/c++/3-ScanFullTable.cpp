// Scans the entirety of the sample-honda6 table

#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h> 
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/AttributeDefinition.h>
#include <aws/dynamodb/model/ScanRequest.h>
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
    Aws::DynamoDB::Model::ScanRequest req;

    // Declare the table we want to search on
    req.SetTableName("honda-hackathon1");

    // This map will hold the last key that we saw on the previous request
    // this allows us to continue resting data until all results have
    // been returned
    Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue> last_key;

    int num_logs = 0;
    do {
        // Get results
        auto error = client.Scan(req);

        // If successful
        if (error.IsSuccess()) {
            auto result = error.GetResult();

            // Separate out the data part of the returned value
            auto items = result.GetItems();

            num_logs += items.size();

            // Iterate over every returned item
            for(auto it = items.begin(); it != items.end(); ++it) {
                std::cout << "driveid: " << (*it).at("driveid").GetN() << "\n";
                std::cout << "logtime: " << (*it).at("logtime").GetN() << "\n";

                // And within every item iterate over every attribute
                for(auto jt = (*it).begin(); jt != (*it).end(); ++jt) {
                    // We dealt with driveid and logtime outside of this loop 
                    // (as they're the composite key for this database), 
                    // so we're ignoring them in this loop
                    if((*jt).first != "driveid" && (*jt).first != "logtime") {
                        Aws::String key = (*jt).first;
                        Aws::DynamoDB::Model::AttributeValue value = (*jt).second;

                        std::cout << "\t" << key << ": ";

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
    // Continue querying the database until we have exhausted it
    while(last_key.size() != 0);

    std::cerr << "\t-- Retrieved " << num_logs << " logs." << std::endl;
    
    // And clean up after ourselves
    Aws::ShutdownAPI(options);

    return 0;
}