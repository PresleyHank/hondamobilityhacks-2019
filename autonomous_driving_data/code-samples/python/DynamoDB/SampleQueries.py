import boto3
import json
import decimal
from pathlib import Path
from boto3.dynamodb.conditions import Key, Attr
from decimal import Decimal
#
# Establish the AWS client connection using access keys.
# Select the correct AWS resource
dynamodb_obj = boto3.resource('dynamodb',
                              aws_access_key_id='***************************',
                              aws_secret_access_key='************************************',
                              region_name='us-east-1'
                              )
# Select the correct data table
table = dynamodb_obj.Table('honda-hackathon1')


# Helper class to convert a DynamoDB item to JSON.
class DecimalEncoder(json.JSONEncoder):
    def default(self, o):
        if isinstance(o, decimal.Decimal):
            if o % 1 != 0:
                return float(o)
            else:
                return int(o)
        return super(DecimalEncoder, self).default(o)


def query_all_gps_data(driveid):
    """
    Function returns GPS data from DynamoDB for a specific driveid.
    :param driveid: A driveid number, that selects a specific drive from the database.
    :return: Returns a dictionary, with items containing GPS_Lat, GPS_Lon, GPS_Alt, and logtime.
    """
    response = table.query(
        ProjectionExpression="GPS_Lat, GPS_Lon, GPS_Alt, logtime",
        KeyConditionExpression=Key('driveid').eq(driveid)
    )
    if 'LastEvaluatedKey' in response:
        cont = True
        cont_key = response['LastEvaluatedKey']
    else:
        cont = False
    while cont:
        response2 = None
        response2 = table.query(
            KeyConditionExpression=Key('driveid').eq(driveid),
            ExclusiveStartKey=cont_key
        )
        response['Items'].extend(response2['Items'])
        if 'LastEvaluatedKey' in response2:
            cont = True
            cont_key = None
            cont_key = response2['LastEvaluatedKey']
        else:
            cont = False
    print_length(response)
    return response


def query_drive_scenario(driveid):
    """
    Function to query all data points for a specific driveid from DynamoDB.
    :param driveid: a single numeric driveid.
    :return: dictionary containing a list of items
    """
    qdsresponse = table.query(
        KeyConditionExpression=Key('driveid').eq(driveid)
    )
    if 'LastEvaluatedKey' in qdsresponse:
        qds_cont = True
        qds_cont_key = qdsresponse['LastEvaluatedKey']
    else:
        qds_cont = False
    while qds_cont:
        qdsresponse2 = None
        qdsresponse2 = table.query(
            KeyConditionExpression=Key('driveid').eq(driveid),
            ExclusiveStartKey=qds_cont_key
        )
        qdsresponse['Items'].extend(qdsresponse2['Items'])
        if 'LastEvaluatedKey' in qdsresponse2:
            qds_cont = True
            qds_cont_key = None
            qds_cont_key = qdsresponse2['LastEvaluatedKey']
        else:
            qds_cont = False
    print_length(qdsresponse)
    return qdsresponse


def query_specific_timestamp(driveid, timestamp):
    """
    :param driveid: a single numeric driveid.
    :param timestamp: a single numeric logtime
    :return: dictionary containing a list of items with a single item
    """
    qstresponse = table.query(
        KeyConditionExpression=Key('driveid').eq(driveid) & Key('logtime').eq(timestamp)
    )
    print_length(qstresponse)
    return qstresponse


def print_length(response):
    qst_count = len(response['Items'])
    if qst_count is not None and qst_count != 0:
        print("        -- Retrieved {} log(s).".format(qst_count))
    else:
        print("Count is invalid/zero.")

def print_log_time(log):
    if log['logtime']:
        print(log['logtime'])
    else:
        print("Log does not contain a logtime.")

def print_gps_data(single_log):
    """
    Function to take in a single gps log item, and print out the coordinates to the terminal log.
    :param single_log: single line of gps log as a list.
    """
    gps_lat = str(single_log['GPS_Lat'])
    gps_lon = str(single_log['GPS_Lon'])
    gps_alt = str(single_log['GPS_Alt'])
    if gps_lat and gps_lon and gps_alt:
        print("Lat: " + gps_lat + ", Lon: " + gps_lon + ", Alt: " + gps_alt)
    else:
        print("Log does not contain valid GPS data.")

if __name__ == '__main__':
    drive_scenario_id = 20181120104743
    sample_timestamp = 1376395

    # Done
    drive_scenario_query_result = query_drive_scenario(drive_scenario_id)
    timestamp_query_result = query_specific_timestamp(drive_scenario_id, sample_timestamp)
    gps_data_only_results = query_all_gps_data(drive_scenario_id)

    print_log_time(drive_scenario_query_result['Items'][0])
    print_log_time(timestamp_query_result['Items'][0])
    print_gps_data(gps_data_only_results['Items'][0])
