from flask import render_template, request
from app.main import bp
import requests


url = "http://192.168.0.119"


@bp.route("/turnOnOff/", methods=["GET", "POST"])
def turnOnOff():
    if request.method == "POST":
        if request.form["turn_on"] == "on":
            requests.post(url + "/turnOn", data={"command": "on"})
            return render_template("turnOnOff.html", command="on")
        elif request.form["turn_on"] == "off":
            requests.post(url + "/turnOn", data={"command": "off"})
            return render_template("turnOnOff.html", command="off")
        else:
            return render_template("turnOnOff.html")
    elif request.method == "GET":
        return render_template("turnOnOff.html")
