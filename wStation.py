from flask import Flask, render_template, request, session, url_for, redirect, abort
import json
import os 
import time
import atexit
import requests

import gspread
import datetime
from oauth2client.service_account import ServiceAccountCredentials

from apscheduler.schedulers.background import BackgroundScheduler
app = Flask(__name__)

# use creds to create a client to interact with the Google Drive API
scope = ['https://spreadsheets.google.com/feeds',
         'https://www.googleapis.com/auth/drive']
creds = ServiceAccountCredentials.from_json_keyfile_name('client_secret.json', scope)
client = gspread.authorize(creds)

#Close where?
sheet = client.open("VML Hourly Data").sheet1
sheet2 = client.open("VML Per Day Accumulations").sheet1



#need to send name
#have status default to zero 
exampleData = {
    "Temperature": 1,
    "pressure": "thing",
    "humidity": "thing",
    "Air Quality ": "thing",
    "Wind Speed": "thing",
    "Wind Heading": "thing",
    "Rain Rate": "thing"

}

#where to put shutdown

@app.route('/')
def hello_world():
    #have to find a way to refresh this data 
    #data = get_data()
    #return data
    return render_template('index.html')

#curl -XPOST -H "Content-Type: application/json" 'http://127.0.0.1:5000/?thing1=1' -d '{"thing2":2}'

#TODO work with multiple weather nodes
#TODO make timing more robust so there is no possibility of desyncronization and rewriting of same data 

@app.route("/get_data", methods=['GET', 'POST'])
def get_data():
    #need to handle empty data here 
    #Get updated data and put it into
    #append current time again 
   url = "https://136.142.64.212:80" 
    
    #Get Fixed IP
    #Url in request will be fixed IP of ethernet 
   r = requests.get(url)
    #data = parse json of r, or in general figure out what r is 
   #data = request.get_json(r)
   

    #params = {
    #    request.get_json().get()
    #}
    #immediately call function to store it in the db 
   # return json.dumps(params)

   exampleD = json.dumps(exampleData)
   writeData(exampleD)
   accum(exampleD)
   return exampleD

#port 80 ip and mac 

@app.route("/per_day", methods = ['GET', 'POST'])
def per_day():
    #take json and put values into array 
    #return set with sum of most recent, only do this when there is new data
    #only called then
    # this just returns the array of the current state of the accumulation  
    return 1

def accum():
    #add jsut gotten weather instance to the accumulation data struct 
    #call write d
    return 1



#Write Data to Google Sheet
#TODO Change this to write only when it gets new data 
#SPlit into current and accumulation sheets 
def writeData(jsonData):
    row = []
    #current time append
    row.append(datetime.datetime.now().strftime("%I:%M%p on %B %d, %Y"))

    data = json.loads(jsonData)

    
    for item in data.items():
        row.append(item[1])

    index = 2
    #error handling?
    #fix where sheet is and how its called 
    sheet.insert_row(row, index)

    return 1

def writePerDay(dataList):
    row = []
    #current time append
    row.append(datetime.datetime.now().strftime("%I:%M%p on %B %d, %Y"))

    data = dataList

    
    for item in data.items():
        row.append(item[1])

    index = 2
    #error handling?
    #fix where sheet is and how its called 
    sheet2.insert_row(row, index)

    return 1



#Schedule Jobs to run every hour
scheduler = BackgroundScheduler()
#For testing purposes
scheduler.add_job(func = get_data, trigger="interval", seconds = 3)

#scheduler.add_job(func = get_data, trigger="interval", minutes = 60)


#originally had writeData as a job
scheduler.start()

#TODO Deprecated for now, original plan was to pull from a database. 
@app.route("/get_histo", methods=['GET', 'POST'])
def get_histo():
    return exampleHistData

#Context Processor
@app.context_processor
def override_url_for():
    return dict(url_for=dated_url_for)

def dated_url_for(endpoint, **values):
    if endpoint == 'static':
        filename = values.get('main.css', None)
        if filename:
            file_path = os.path.join(app.root_path,
                                 endpoint, filename)
            values['q'] = int(os.stat(file_path).st_mtime)
    return url_for(endpoint, **values)



