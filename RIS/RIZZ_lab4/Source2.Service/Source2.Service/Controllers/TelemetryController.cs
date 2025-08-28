using Microsoft.AspNetCore.Mvc;

namespace Source2.Service.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class TelemetryController : ControllerBase
    {
        private readonly ILogger<TelemetryController> _logger;
        private readonly Random _random = new Random();

        public TelemetryController(ILogger<TelemetryController> logger)
        {
            _logger = logger;
        }

        [HttpGet]
        public IActionResult GetTelemetry()
        {
            _logger.LogInformation("GetTelemetry request received at Source2");

            var telemetryDataList = new List<TelemetryData>();

            // Генерируем несколько измерений
            for (int i = 0; i < 3; i++)
            {
                var telemetryData = new TelemetryData
                {
                    SourceId = "Source2",  // *** ВАЖНО: Source2 ***
                    Timestamp = DateTime.UtcNow,
                    MeasurementType = i == 0 ? "BatteryLevel" : (i == 1 ? "SignalStrength" : "GPSQuality"),
                    Unit = i == 0 ? "%" : (i == 1 ? "dBm" : "Stars")
                };

                // Эмулируем ошибку (20% вероятности)
                if (_random.Next(10) < 2)
                {
                    telemetryData.Status = "Error";
                    telemetryData.ErrorMessage = "Sensor failure.";
                    _logger.LogError("Error occurred while reading sensor data at Source2");
                }
                else
                {
                    telemetryData.Value = _random.Next(0, 100); // Пример: Заряд батареи
                }

                telemetryDataList.Add(telemetryData);
            }

            return Ok(telemetryDataList);
        }
    }
}
