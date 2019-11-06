from flask import Flask, render_template, request, session, url_for, redirect, abort
import json
import os 
app = Flask(__name__)

#need to send name
#have status default to zero 
exampleData = {
    "temp" : 34,
    "pressure" : 50,
    "humidity" : 45
    }

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
    return exampleData
    #params = {
     #   'weather': request.get_json().get('weather')
    #}
    #immediately call function to store it in the db 
    #return json.dumps(params)

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