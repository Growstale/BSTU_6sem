using Central.Service.Models;
using System.Collections.Generic;

namespace Central.Service.Services
{
    public class TelemetryBackgroundService : BackgroundService
    {
        private readonly IServiceProvider _serviceProvider;
        private readonly IConfiguration _configuration;
        private readonly ILogger<TelemetryBackgroundService> _logger;
        private static readonly List<TelemetryData> _telemetryDataStore = new List<TelemetryData>(); // Хранилище данных

        public TelemetryBackgroundService(IServiceProvider serviceProvider, IConfiguration configuration, ILogger<TelemetryBackgroundService> logger)
        {
            _serviceProvider = serviceProvider ?? throw new ArgumentNullException(nameof(serviceProvider));
            _configuration = configuration ?? throw new ArgumentNullException(nameof(configuration));
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
        }

        protected override async Task ExecuteAsync(CancellationToken stoppingToken)
        {
            _logger.LogInformation("TelemetryBackgroundService started.");

            while (!stoppingToken.IsCancellationRequested)
            {
                try
                {
                    using (var scope = _serviceProvider.CreateScope())
                    {
                        var telemetryService = scope.ServiceProvider.GetRequiredService<ITelemetryService>();
                        List<string> sourceUrls = new List<string>();
                        _configuration.GetSection("TelemetrySources").Bind(sourceUrls);

                        foreach (var sourceUrl in sourceUrls)
                        {
                            if (await telemetryService.IsSourceAvailableAsync(sourceUrl))
                            {
                                var telemetryData = await telemetryService.GetTelemetryDataAsync(sourceUrl);
                                if (telemetryData != null)
                                {
                                    // Фильтруем "плохие" записи
                                    var validTelemetryData = telemetryData.Where(data => data.Status != "Error").ToList();

                                    lock (_telemetryDataStore) // Безопасность при многопоточном доступе
                                    {
                                        _telemetryDataStore.AddRange(validTelemetryData);
                                    }
                                }
                            }
                            else
                            {
                                _logger.LogWarning($"Source {sourceUrl} is unavailable.");
                            }

                        }
                    }
                }
                catch (Exception ex)
                {
                    _logger.LogError(ex, "An error occurred during telemetry data collection.");
                }

                await Task.Delay(TimeSpan.FromSeconds(10), stoppingToken); // Опрашиваем каждые 10 секунд
            }

            _logger.LogInformation("TelemetryBackgroundService stopped.");
        }

        public static List<TelemetryData> GetAllTelemetryData()
        {
            lock (_telemetryDataStore)
            {
                return new List<TelemetryData>(_telemetryDataStore); // Возвращаем копию, чтобы избежать проблем с многопоточностью
            }
        }
    }
}
