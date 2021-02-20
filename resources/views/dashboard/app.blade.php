@extends('layouts.app')
@section('title', 'Dashboard | ' . config('app.name'))
@push('head.css')
    <link href="{{ asset('css/dashboard/app.css') }}?v=1.0.0" rel="stylesheet">
@endpush
@section('body.content')
    <div id="app"></div>
@endsection
@push('body.js')
    <script src="{{ mix('js/dashboard/app.js') }}?v=4.1.1"></script>
@endpush
