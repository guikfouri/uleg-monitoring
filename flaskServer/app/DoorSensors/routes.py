from flask import render_template, request
from app.extensions import db
from app.main import bp
from app.models.sensor import Sensor, Read


url = "http://192.168.0.119"


@bp.route("/showDoorHistoric/<sensor_id>/")
def showDoorHistoric(sensor_id):
    sensor = Sensor.query.get(sensor_id)

    return render_template("DoorSensors/doorHistoric.html", sensor=sensor)


@bp.route("/setDoorStatus/", methods=["POST"])
def setDoorStatus():
    request_data = request.get_json()

    sensor_id = request_data["sensor_id"]
    status = request_data["status"]

    new_read = Read(value=status, sensor_id=sensor_id)

    db.session.add(new_read)
    db.session.commit()

    return "Added read {} for sensor {}".format(status, sensor_id)
