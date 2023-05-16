from flask import Blueprint

bp = Blueprint("IRSensors", __name__)

from app.IRSensors import routes
