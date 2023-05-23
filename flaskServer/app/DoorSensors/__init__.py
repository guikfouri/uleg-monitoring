from flask import Blueprint

bp = Blueprint("doorSensors", __name__)

from app.DoorSensors import routes
