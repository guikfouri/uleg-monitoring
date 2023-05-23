from flask import render_template, request, redirect
from app.main import bp
from app.extensions import db
from app.models.sensor import Sensor


@bp.route("/")
def index():
    sensors = Sensor.query.all()
    return render_template("index.html", sensors=sensors)


@bp.route("/addSensor/", methods=["GET", "POST"])
def addSensor():
    if request.method == "POST":
        new_sensor = Sensor(
            name=request.form["name"],
            type=request.form["type"],
            macAddress=request.form["macAddress"],
        )
        db.session.add(new_sensor)
        db.session.commit()

        return redirect("/")
    elif request.method == "GET":
        return render_template("addSensor.html")


@bp.route("/deleteSensor/<sensor_id>/", methods=["POST"])
def deleteSensor(sensor_id):
    deleted_sensor = Sensor.query.get(sensor_id)
    db.session.delete(deleted_sensor)
    db.session.commit()

    return redirect("/")
