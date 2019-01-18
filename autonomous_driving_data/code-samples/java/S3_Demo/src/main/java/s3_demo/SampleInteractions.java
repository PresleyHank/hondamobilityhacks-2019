package s3_demo;

import com.amazonaws.AmazonClientException;
import com.amazonaws.AmazonServiceException;
import com.amazonaws.auth.AWSStaticCredentialsProvider;
import com.amazonaws.auth.BasicAWSCredentials;
import com.amazonaws.regions.Regions;
import com.amazonaws.services.s3.AmazonS3;
import com.amazonaws.services.s3.AmazonS3Client;
import com.amazonaws.services.s3.AmazonS3ClientBuilder;
import com.amazonaws.services.s3.model.GetObjectRequest;
import com.amazonaws.services.s3.model.S3Object;
import com.amazonaws.services.s3.model.S3ObjectInputStream;

import javax.swing.plaf.synth.Region;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class SampleInteractions {
    private static AmazonS3 client;
    private static String BUCKET_NAME = "p3na-18gus.3101.027";
    private static String ACCESS_KEY = "YOUR_ACCESS_KEY_HERE";
    private static String SECRET_KEY = "YOUR_SECRET_KEY_HERE";

    /**
     * Creates the AWS S3 client, which is used to access the AWS API
     *
     * Requires an acces key and secret key, as well as defining the AWS region. Those credentials will be provided by
     * the hackathon organizers
     */
    private static void createClient() {
        BasicAWSCredentials awsCredentials = new BasicAWSCredentials(ACCESS_KEY, SECRET_KEY);
        client = AmazonS3ClientBuilder.standard()
                .withCredentials(new AWSStaticCredentialsProvider(awsCredentials))
                .withRegion(Regions.US_EAST_1)
                .build();
    }

    public static void main(String[] args) {
        // initialize our client
        createClient();

        String objectPath = "video-files/";
        String objectName = "Recfile P3 Edge 20181120 104743 Webcam Driver Outputiplimage.m4v";
        String objectKey = objectPath + objectName;
        String saveFilePath = "local/file/path/" + objectName;

        try {
            // call download file method
            downloadFile(objectKey, saveFilePath);
        }catch (AmazonServiceException ase) {
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
     * Downloads the file at the specified key in a bucket and saves it at the specified location
     * @param objectKey The key of the object, which includes the path from the bucket and the file name
     * @param saveFilePath where you want to save the file to on your computer
     */
    private static void downloadFile(String objectKey, String saveFilePath) {
        // create S3Object with the bucket name and object's key
        S3Object object = client.getObject(BUCKET_NAME, objectKey);
        S3ObjectInputStream inputStream = object.getObjectContent();
        try {
            // create file stream to a local file
            FileOutputStream fileOutputStream = new FileOutputStream(new File(saveFilePath));
            byte[] read_buf = new byte[1024];
            int read_len;
            // write the stream
            while((read_len = inputStream.read(read_buf)) > 0) {
                fileOutputStream.write(read_buf, 0, read_len);
            }
            // close the streams
            inputStream.close();
            fileOutputStream.close();
        } catch(IOException e) {
            System.out.println(e.getMessage());
        }
    }
}
