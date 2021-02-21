@extends('layouts.app')
@section('title', 'Dashboard | ' . config('app.name'))
@push('head.css')
    @php
        $appCssFile = mix('/css/dashboard/app.css');
    @endphp
    <link href="{{ $appCssFile }}?v={{ filemtime($appCssFile) }}" rel="stylesheet">
@endpush
@section('body.content')
    <div id="app"></div>
@endsection
@push('body.js')
    @php
        $appJsFile = mix('/js/dashboard/app.js');
    @endphp
    <script src="{{ $appJsFile }}?v={{ filemtime($appJsFile) }}"></script>
@endpush
