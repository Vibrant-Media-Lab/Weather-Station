from flask import Flask, render_template, request, session, url_for, redirect, abort
import json
import os 
import time
import atexit

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

# Find a workbook by name and open the first sheet
# Make sure you use the right name here.
sheet = client.open("VML WeatherStation Historical Data").sheet1



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

exampleHistData = [{
                "Temperature": 1,
                "pressure": "thing",
                "humidity": "thing",
                "Air Quality ": "thing",
                "Wind Speed": "thing",
                "Wind Heading": "thing",
                "Rain Rate": "thing"
            },
            {
                "Temperature": 1,
                "pressure": "thing",
                "humidity": "thing",
                "Air Quality ": "thing",
                "Wind Speed": "thing",
                "Wind Heading": "thing",
                "Rain Rate": "thing"
            },
            {
                "Temperature": 1,
                "pressure": "thing",
                "humidity": "thing",
                "Air Quality ": "thing",
                "Wind Speed": "thing",
                "Wind Heading": "thing",
                "Rain Rate": "thing"
            }
]




#where to put shutdown

@app.route('/')
def hello_world():
    #have to find a way to refresh this data 
    #data = get_data()
    #return data
    return render_template('index.html')

#curl -XPOST -H "Content-Type: application/json" 'http://127.0.0.1:5000/?thing1=1' -d '{"thing2":2}'

#Two things activate this, doing a refresh 
#Or having a count down until the next get - or at least itme stamp it 
#Eventually have to make this work with multiple nodes 
@app.route("/get_data", methods=['GET', 'POST'])
def get_data():
    #need to handle empty data here 

    #Get updated data and put it into
    #append current time again  
    
    #params = {
    #    request.get_json().get()
    #}
    #immediately call function to store it in the db 
   # return json.dumps(params)
   exampleD = json.dumps(exampleData)
   return exampleD

scheduler = BackgroundScheduler()
scheduler.add_job(func = get_data, trigger="interval", minutes = 60)




#Write Data to Google Sheet
def writeData():
    row = []
    #current time append
    row.append(datetime.datetime.now().strftime("%I:%M%p on %B %d, %Y"))

    data = json.loads(get_data())

    
    for item in data.items():
        row.append(item[1])

    index = 2
    #error handling?
    sheet.insert_row(row, index)

    return 1

scheduler.add_job(func = writeData, trigger="interval", minutes = 60)
scheduler.start()

@app.route("/get_histo", methods=['GET', 'POST'])
def get_histo():
    return exampleHistData

# handle hsitory and store it 

#Get input event, how to monitor post requests
#Justify why baseline is better 

#What it has logged
    #summed up from the last 24 hours, long term logging
    #csv on local sd card
    #pushed to google doc to put everything together
    #link to this on page to see - have some kind of database 
    #sql lite 
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


#db controller and write to database with time stamp
