using Central.Service.Models;
using System.Text.Json;

namespace Central.Service.Services
{
   public class TelemetryService : ITelemetryService
    {
        private readonly IHttpClientFactory _httpClientFactory;
        private readonly ILogger<TelemetryService> _logger;

        public TelemetryService(IHttpClientFactory httpClientFactory, ILogger<TelemetryService> logger)
        {
            _httpClientFactory = httpClientFactory ?? throw new ArgumentNullException(nameof(httpClientFactory));
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
        }

        public async Task<List<TelemetryData>> GetTelemetryDataAsync(string sourceUrl)
        {
            try
            {
                _logger.LogInformation($"Fetching telemetry data from: {sourceUrl}");
                var httpClient = _httpClientFactory.CreateClient();
                var response = await httpClient.GetAsync(sourceUrl);

                if (response.IsSuccessStatusCode)
                {
                    var content = await response.Content.ReadAsStringAsync();
                    var options = new JsonSerializerOptions { PropertyNameCaseInsensitive = true }; // Для игнорирования регистра
                    var telemetryData = JsonSerializer.Deserialize<List<TelemetryData>>(content, options);

                    if (telemetryData != null)
                    {
                        _logger.LogInformation($"Successfully fetched {telemetryData.Count} telemetry entries from {sourceUrl}");
                        return telemetryData;
                    }
                    else
                    {
                        _logger.LogError($"Failed to deserialize telemetry data from {sourceUrl}. Content: {content}");
                        return new List<TelemetryData>(); // Или выбросить исключение
                    }
                }
                else
                {
                    _logger.LogError($"Failed to fetch telemetry data from {sourceUrl}. Status code: {response.StatusCode}");
                    return new List<TelemetryData>(); // Или выбросить исключение
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"An error occurred while fetching telemetry data from {sourceUrl}");
                return new List<TelemetryData>(); // Или выбросить исключение
            }
        }

        public async Task<bool> IsSourceAvailableAsync(string sourceUrl)
        {
            try
            {
                var httpClient = _httpClientFactory.CreateClient();
                var response = await httpClient.GetAsync(sourceUrl);
                return response.IsSuccessStatusCode;
            }
            catch (Exception)
            {
                return false; // Считаем недоступным при любой ошибке
            }
        }
    }
}
