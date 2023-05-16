from app.extensions import db


class Sensor(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    label = db.Column(db.String(100), nullable=False)
    type = db.Column(db.Integer, nullable=False)
    mac = db.Column(db.String(100), nullable=False)
    readings = db.relationship("Read", backref="post")
    created_at = db.Column(db.DateTime(timezone=True), server_default=db.func.now())

    def __repr__(self):
        return f"<Sensor {self.label}>"


class Read(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    value = db.Column(db.Integer)
    created_at = db.Column(db.DateTime(timezone=True), server_default=db.func.now())
    sensor_id = db.Column(db.Integer, db.ForeignKey("sensor.id"))

    def __repr__(self):
        return f"<Read {self.value} from sensor {self.sensor_id}>"
