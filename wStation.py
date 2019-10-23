from flask import Flask, render_template, request, session, url_for, redirect, abort
app = Flask(__name__)

@app.route('/')
def hello_world():
    return render_template('index.html')

@app.route("/get_data", methods=['GET'])
def get_data():
    return "DATA"
