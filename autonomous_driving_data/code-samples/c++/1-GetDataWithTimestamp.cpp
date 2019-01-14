// Gets all data associated with a particular driveid and logtime

#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h> 
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/AttributeDefinition.h>
#include <aws/dynamodb/model/GetItemRequest.h>
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
    Aws::DynamoDB::Model::GetItemRequest req;

    // And the attributes we'll be retrieving based on
    Aws::DynamoDB::Model::AttributeValue timestamp;
    Aws::DynamoDB::Model::AttributeValue drive_id;

    // Searching in table honda-hackathon1 for Timestamps that match 1039012
    req.SetTableName("honda-hackathon1");

    // Assign values to our AttributeValue's and add them to our search
    timestamp.SetN("1376395");
    req.AddKey("logtime", timestamp);
    drive_id.SetN("20181120104743");
    req.AddKey("driveid", drive_id);

    // Get results
    auto result = client.GetItem(req);

    int num_logs = 1;
    bool first_loop = true;
    
    Aws::Map<Aws::String, Aws::DynamoDB::Model::AttributeValue> first_item;
    // If successful
    if (result.IsSuccess()) {
        auto item = result.GetResult().GetItem();
        
        if(first_loop) {
            first_loop = false;
            first_item = item;
        }

        // Iterate over every attribute of the item
        for(auto it = item.begin(); it != item.end(); ++it) {
            // And do something with it, here, we're just
            // printing to the screen
            Aws::String key = (*it).first;
            Aws::DynamoDB::Model::AttributeValue value = (*it).second;

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
    // Else tell us what went wrong
    else
        std::cout << "ERROR: " << result.GetError().GetMessage() << std::endl;

    // And clean up after ourselves
    Aws::ShutdownAPI(options);
    
    std::cerr << "\t-- Retrieved " << num_logs << " logs." << std::endl;
    std::cerr << first_item.at("logtime").GetN() << std::endl;

    return 0;
}