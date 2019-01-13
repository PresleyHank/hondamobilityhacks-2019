// Gets all data associated with a particular driveid and logtime

#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h> 
#include <aws/s3/S3Client.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <fstream>

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
    Aws::S3::S3Client client(cred, config);

    // The request we'll be making to the client
    Aws::S3::Model::GetObjectRequest req;

    // Searching in bucket p3na-18gus.3101.027 for the item with the specified key
    req.SetBucket("p3na-18gus.3101.027");
    req.SetKey("video-files/Recfile P3 Edge 20181120 133940 Webcam Driver Outputiplimage.m4v");

    // Get the item
    auto request = client.GetObject(req);

    // If successful
    if (request.IsSuccess()) {
        std::string out_location = "outfile.m4v";
        Aws::OFStream local_file;
        std::cout << "Writing " << out_location << " to disk..." << std::endl;
        local_file.open(out_location.c_str(), std::ios::out | std::ios::binary);
        local_file << request.GetResult().GetBody().rdbuf();
        local_file.close();
        std::cout << "Done!" << std::endl;
    }
    // Else tell us what went wrong
    else
        std::cout << "ERROR: " << request.GetError().GetMessage() << std::endl;

    // And clean up after ourselves
    Aws::ShutdownAPI(options);

    return 0;
}