from flask import Flask

from config import Config
from app.extensions import db


def create_app(config_class=Config):
    app = Flask(__name__)
    app.config.from_object(config_class)

    # Initialize Flask extensions here
    db.init_app(app)

    # Register blueprints here
    from app.main import bp as main_bp
    from app.IRSensors import bp as irSensors_bp

    app.register_blueprint(main_bp)
    app.register_blueprint(irSensors_bp, url_prefix="/irSensors")

    return app
