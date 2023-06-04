from flask import abort, render_template, request
from app.extensions import db
from app.main import bp
from app.models.sensor import Sensor, Read
from sqlalchemy.exc import NoResultFound


@bp.route("/showDoorHistoric/<sensor_id>/")
def showDoorHistoric(sensor_id):
    sensor = Sensor.query.get(sensor_id)

    return render_template("DoorSensors/doorHistoric.html", sensor=sensor)


@bp.route("/setDoorStatus/", methods=["POST"])
def setDoorStatus():
    request_data = request.get_json()

    macAddress = request_data["macAddress"]
    status = request_data["status"]

    try:
        sensor = Sensor.query.filter(Sensor.macAddress == macAddress.upper()).one()
        sensor_id = sensor.id

        new_read = Read(value=status, sensor_id=sensor_id)

        db.session.add(new_read)
        db.session.commit()

        return "Added read {} for sensor {}".format(status, sensor_id)
    except NoResultFound:
        abort(404)
    except Exception as e:
        print(e)
        abort(500)
