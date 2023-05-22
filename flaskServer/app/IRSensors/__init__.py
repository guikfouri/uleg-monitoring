from flask import Blueprint

bp = Blueprint("irSensors", __name__)

from app.IRSensors import routes
