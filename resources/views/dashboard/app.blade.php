<!DOCTYPE html>
<html lang="{{ str_replace('_', '-', app()->getLocale()) }}">
    <head>
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <meta name="csrf-token" content="{{ csrf_token() }}">
        <title>{{ config('app.name', 'Dashboard') }}</title>
        <link rel="icon" href="{{ asset('favicon.ico') }}">
        <link href="{{ asset('css/dashboard/app.css') }}?v=1.0.0" rel="stylesheet">
    </head>
    <body>
        <div id="app"></div>
        <script src="{{ mix('js/dashboard/app.js') }}?v=1.0.0"></script>
    </body>
</html>