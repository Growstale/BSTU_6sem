using Central.Service.Models;

namespace Central.Service.Services
{
    public interface ITelemetryService
    {
        Task<List<TelemetryData>> GetTelemetryDataAsync(string sourceUrl);
        Task<bool> IsSourceAvailableAsync(string sourceUrl);
    }
}