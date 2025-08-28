using Central.Service.Models;
using Central.Service.Services;
using Microsoft.AspNetCore.Mvc;

namespace Central.Service.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class CentralTelemetryController : ControllerBase
    {
        private readonly ITelemetryService _telemetryService;
        private readonly IConfiguration _configuration;
        private readonly ILogger<CentralTelemetryController> _logger;

        public CentralTelemetryController(ITelemetryService telemetryService, IConfiguration configuration, ILogger<CentralTelemetryController> logger)
        {
            _telemetryService = telemetryService ?? throw new ArgumentNullException(nameof(telemetryService));
            _configuration = configuration ?? throw new ArgumentNullException(nameof(configuration));
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
        }

        [HttpGet("all")]
        public IActionResult GetAllTelemetry()
        {
            _logger.LogInformation("GetAllTelemetry request received.");
            var allData = TelemetryBackgroundService.GetAllTelemetryData();
            return Ok(allData);
        }

        [HttpGet("sources/status")]
        public async Task<IActionResult> GetSourcesStatus()
        {
            _logger.LogInformation("GetSourcesStatus request received.");

            List<string> sourceUrls = new List<string>();
            _configuration.GetSection("TelemetrySources").Bind(sourceUrls);

            var sourcesStatus = new Dictionary<string, bool>();
            foreach (var sourceUrl in sourceUrls)
            {
                sourcesStatus[sourceUrl] = await _telemetryService.IsSourceAvailableAsync(sourceUrl);
            }

            return Ok(sourcesStatus);
        }
    }
}
