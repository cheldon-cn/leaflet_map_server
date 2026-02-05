# Dynamic Map Image Service - Web Frontend

A modern, responsive web frontend for the Dynamic Map Image Service, built with Leaflet, Vite, and vanilla JavaScript.

## Features

- **Interactive Map**: Leaflet-based map with smooth panning and zooming
- **Dynamic Image Generation**: On-demand map image generation from the C++ backend
- **Layer Management**: Add, remove, and reorder map layers
- **Real-time Updates**: Automatic map updates on viewport changes
- **Performance Optimized**: Caching, debouncing, and concurrent request management
- **Responsive Design**: Works on desktop, tablet, and mobile devices
- **Accessibility**: ARIA attributes, keyboard navigation, and screen reader support

## Prerequisites

- Node.js 16+
- The C++ backend server running (see server README)
- Modern web browser with ES2020 support

## Installation

1. Clone the repository:
```bash
git clone <repository-url>
cd leaf/web
```

2. Install dependencies:
```bash
npm install
```

3. Configure the backend URL (optional):
Create a `.env` file in the `web` directory:
```env
VITE_API_BASE_URL=http://localhost:8080
```

## Development

Run the development server:
```bash
npm run dev
```

This will start the Vite development server on `http://localhost:3000` with hot reload.

## Building for Production

Build the project:
```bash
npm run build
```

The built files will be in the `dist` directory. You can preview the build with:
```bash
npm run preview
```

## Project Structure

```
web/
├── public/                 # Static assets
│   ├── index.html         # Main HTML file
│   ├── favicon.ico        # Site favicon
│   └── robots.txt         # Robots configuration
├── src/
│   ├── app.js             # Application entry point
│   ├── assets/            # Static assets (CSS, images)
│   ├── components/        # Reusable UI components
│   │   ├── Map/           # Map-related components
│   │   ├── MapControl/    # Map control components
│   │   └── UI/            # General UI components
│   ├── config/            # Configuration files
│   ├── services/          # Business logic services
│   │   ├── api/          # API communication
│   │   ├── cache/        # Caching services
│   │   └── utils/        # Utility functions
│   └── stores/           # State management
├── package.json           # Dependencies and scripts
├── vite.config.js         # Vite configuration
└── README.md              # This file
```

## Key Components

### DynamicImageLayer
Manages the dynamic map image overlay, handling viewport changes and image updates.

### MapApiClient
Handles communication with the C++ backend API, including request/response processing and error handling.

### ImageCache
Coordinates between memory and localStorage caching strategies to optimize performance.

### State Stores
- `mapStore`: Manages map viewport and settings
- `layerStore`: Manages layer configuration and state
- `requestStore`: Tracks API requests and performance metrics

## Configuration

Configuration is managed through `src/config/settings.js`. Key settings include:

- `API_BASE_URL`: Backend server URL
- `MAP`: Default map center, zoom, and bounds
- `RENDER`: Image quality and size settings
- `CACHE`: Caching strategies and TTL
- `PERFORMANCE`: Debounce delays and concurrent request limits

Environment-specific configurations can be set in `.env` files.

## API Integration

The frontend communicates with the backend via HTTP API. Key endpoints:

- `POST /generate`: Generate map image for given viewport
- `GET /tile/{z}/{x}/{y}.png`: Get map tile
- `GET /layers`: List available layers
- `GET /health`: Check server health

## Caching Strategy

Three caching strategies are supported:

1. **Aggressive**: Large memory cache, aggressive localStorage, skips network when possible
2. **Balanced**: Moderate caching with fallback to network (default)
3. **None**: No caching, always fetch from network

## Browser Support

- Chrome 90+
- Firefox 88+
- Safari 14+
- Edge 90+

## Performance Tips

1. **Use appropriate cache strategy**: Aggressive for stable connections, balanced for mixed
2. **Optimize image size**: Request appropriate dimensions for your use case
3. **Layer management**: Disable unnecessary layers
4. **Network conditions**: Adjust settings based on connection speed

## Development Guidelines

1. **Code Style**: Follow existing patterns and ESLint rules
2. **Testing**: Write unit tests for new functionality
3. **Performance**: Profile changes and optimize critical paths
4. **Accessibility**: Ensure all new features are keyboard navigable and screen reader friendly

## Troubleshooting

### Backend Connection Issues
- Ensure the backend server is running
- Check the `API_BASE_URL` configuration
- Verify CORS settings on the backend

### Performance Issues
- Check network conditions
- Adjust cache strategy
- Reduce image size or quality

### Browser Compatibility
- Use polyfills for older browsers if needed
- Check console for errors
- Verify feature support

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make changes with appropriate tests
4. Submit a pull request

## License

MIT License - see LICENSE file for details.

## Support

For issues and questions:
1. Check the troubleshooting section
2. Search existing issues
3. Create a new issue with detailed information