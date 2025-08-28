namespace Source2.Service
{
    public class TelemetryData
    {
        public string SourceId { get; set; } = "Source2";
        public DateTime Timestamp { get; set; }
        public string MeasurementType { get; set; }
        public double Value { get; set; }
        public string Unit { get; set; }
        public string Status { get; set; } = "OK"; // По умолчанию - OK
        public string ErrorMessage { get; set; } // Сообщение об ошибке (если есть)
    }
}
