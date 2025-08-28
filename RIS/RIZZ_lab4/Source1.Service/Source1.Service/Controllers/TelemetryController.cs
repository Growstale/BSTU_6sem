using Microsoft.AspNetCore.Mvc;

namespace Source1.Service.Controllers
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
            _logger.LogInformation("GetTelemetry request received at Source1");

            var telemetryDataList = new List<TelemetryData>();

            // Генерируем несколько измерений
            for (int i = 0; i < 3; i++)
            {
                var telemetryData = new TelemetryData
                {
                    SourceId = "Source1",
                    Timestamp = DateTime.UtcNow,
                    MeasurementType = i == 0 ? "Temperature" : (i == 1 ? "Pressure" : "Voltage"),
                    Unit = i == 0 ? "°C" : (i == 1 ? "kPa" : "V")
                };

                // Эмулируем ошибку (20% вероятности)
                if (_random.Next(10) < 2)
                {
                    telemetryData.Status = "Error";
                    telemetryData.ErrorMessage = "Failed to read sensor data.";
                    _logger.LogError("Error occurred while reading sensor data at Source1");
                }
                else
                {
                    telemetryData.Value = _random.Next(20, 30); // 20-30 для температуры, например
                }
                telemetryDataList.Add(telemetryData);
            }

            return Ok(telemetryDataList);
        }
    }
}
