from flask import render_template, request, redirect
from app.main import bp
from app.models.sensor import Sensor
import requests


url = "http://192.168.0.123"

COMMAND_ON = "on"
COMMAND_OFF = "off"


@bp.route("/turnOnOff/<int:sensor_id>/", methods=["GET", "POST"])
def turnOnOff(sensor_id):
    sensor = Sensor.query.get(sensor_id)
    if request.method == "POST":
        if request.form["turn_on"] == COMMAND_ON:
            requests.post(
                url + "/turnOn",
                data={"command": COMMAND_ON, "macAddress": sensor.macAddress},
            )

            return render_template(
                "IRSensors/turnOnOff.html", command=COMMAND_ON, sensor_id=sensor_id
            )

        elif request.form["turn_on"] == COMMAND_OFF:
            requests.post(
                url + "/turnOn",
                data={"command": COMMAND_OFF, "macAddress": sensor.macAddress},
            )

            return render_template(
                "IRSensors/turnOnOff.html", command=COMMAND_OFF, sensor_id=sensor_id
            )

        else:
            return redirect("/turnOnOff/{}".format(sensor_id))

    elif request.method == "GET":
        return render_template("IRSensors/turnOnOff.html", sensor_id=sensor_id)


@bp.route("/turnOn/<int:sensor_id>/")
def turnOn(sensor_id):
    sensor = Sensor.query.get(sensor_id)
    requests.post(
        url + "/turnOn", data={"command": COMMAND_ON, "macAddress": sensor.macAddress}
    )

    return redirect("/turnOnOff/{}".format(sensor_id))


@bp.route("/turnOff/<int:sensor_id>/")
def turnOff(sensor_id):
    sensor = Sensor.query.get(sensor_id)
    requests.post(
        url + "/turnOn",
        data={"command": COMMAND_OFF, "macAddress": sensor.macAddress},
    )

    return redirect("/turnOnOff/{}".format(sensor_id))
