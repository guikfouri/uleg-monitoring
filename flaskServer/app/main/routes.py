from flask import render_template, request
from app.main import bp
from app.extensions import db
from app.models.sensor import Sensor


@bp.route("/")
def index():
    return render_template("index.html")


@bp.route("/addSensor/", methods=["GET", "POST"])
def addSensor():
    if request.method == "POST":
        new_sensor = Sensor(
            label=request.form["label"],
            type=request.form["type"],
            mac=request.form["mac"],
        )
        db.session.add(new_sensor)
        db.session.commit()
        return render_template("addSensor.html")
    elif request.method == "GET":
        return render_template("addSensor.html")
