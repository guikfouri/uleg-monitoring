from flask import render_template, request
from app.main import bp
from app.models.sensor import Sensor
import requests


url = "http://192.168.0.119"


@bp.route("/turnOnOff/<int:sensor_id>/", methods=["GET", "POST"])
def turnOnOff(sensor_id):
    sensor = Sensor.query.get(sensor_id)
    if request.method == "POST":
        if request.form["turn_on"] == "on":
            # requests.post(
            #     url + "/turnOn", data={"command": "on", "macAddress": sensor.macAddress}
            # )
            return sensor.macAddress
        elif request.form["turn_on"] == "off":
            # requests.post(
            #     url + "/turnOn",
            #     data={"command": "off", "macAddress": sensor.macAddress},
            # )
            return render_template(
                "IRSensors/turnOnOff.html", command="off", sensor_id=sensor_id
            )
        else:
            return render_template("turnOnOff.html")
    elif request.method == "GET":
        return render_template("IRSensors/turnOnOff.html", sensor_id=sensor_id)


@bp.route("/turnOn/<int:sensor_id>/")
def turnOn(sensor_id):
    # requests.post(url + "/turnOn", data={"command": "on"})
    return render_template("IRSensors/turnOnOff.html", sensor_id=sensor_id)


@bp.route("/turnOff/<int:sensor_id>/")
def turnOff(sensor_id):
    # requests.post(url + "/turnOff", data={"command": "off"})
    return render_template("IRSensors/turnOnOff.html", sensor_id=sensor_id)
