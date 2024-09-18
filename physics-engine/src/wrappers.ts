export class Inlet{
    constructor(i:Box2D.b2Inlet){
        this.inlet=i;
    }
    private inlet:Box2D.b2Inlet;
    activate():void{
        this.inlet.Activate();
    }
    deactivate():void{
        this.inlet.Deactivate();
    }
}

export class Sensor{
    constructor(s:Box2D.b2Sensor){
        this.sensor=s;
    }
    private sensor:Box2D.b2Sensor;

    getPressure(){
        return this.sensor.GetAvgPressure();
    }
    getSpeed(){
        return this.sensor.GetAvgSpeed();
    }
}